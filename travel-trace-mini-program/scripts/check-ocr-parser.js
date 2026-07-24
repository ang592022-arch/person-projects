const fs = require('fs');
const os = require('os');
const path = require('path');
const { spawnSync } = require('child_process');

const root = path.resolve(__dirname, '..');
const compilerCandidates = [
  path.join(root, '.codex-tools', 'typescript-5.9.3', 'lib', 'tsc.js'),
  path.join(root, 'node_modules', 'typescript', 'lib', 'tsc.js'),
];
const compiler = compilerCandidates.find((candidate) => fs.existsSync(candidate));

if (!compiler) {
  console.error('TypeScript compiler not found. Install TypeScript or restore .codex-tools/typescript-5.9.3.');
  process.exit(1);
}

const output = fs.mkdtempSync(path.join(os.tmpdir(), 'travel-trace-ocr-'));

try {
  const compile = spawnSync(process.execPath, [
    compiler,
    '-p',
    path.join(root, 'tsconfig.json'),
    '--noEmit',
    'false',
    '--outDir',
    output,
    '--module',
    'commonjs',
  ], {
    cwd: root,
    encoding: 'utf8',
  });

  if (compile.status !== 0) {
    process.stdout.write(compile.stdout || '');
    process.stderr.write(compile.stderr || '');
    process.exitCode = compile.status || 1;
  } else {
    global.wx = {
      getStorageSync: () => [],
      setStorageSync: () => undefined,
      removeStorageSync: () => undefined,
    };

    const { parseTravelImportText } = require(path.join(output, 'services', 'ocr-import.service.js'));
    const cases = [
      {
        name: 'Ctrip China city and date',
        text: 'Ctrip\n2025\u5e747\u67081\u65e5\n\u62c9\u8428',
        check: (result) => result.visitDate === '2025-07-01'
          && result.detectedPlaces.some((place) => place.cityName.includes('\u62c9\u8428')),
      },
      {
        name: 'Known world city is deduplicated',
        text: '\u4e1c\u4eac \u65e5\u672c\n2026-03-02',
        check: (result) => result.detectedPlaces.length === 1
          && result.detectedPlaces[0].regionCode === 'world-city:japan:tokyo',
      },
      {
        name: 'Flight number and two cities',
        text: '\u822a\u73ed CA1234\n\u5317\u4eac \u2192 \u4e0a\u6d77\n2025-08-03',
        check: (result) => result.tickets[0]?.type === 'flight'
          && result.tickets[0]?.ticketNo === 'CA1234'
          && result.detectedPlaces.length === 2,
      },
      {
        name: 'Unknown foreign city becomes a reviewed custom candidate',
        text: 'Ushuaia Argentina\n2026-01-12',
        check: (result) => result.detectedPlaces.some((place) => (
          place.isCustomCandidate
          && place.countryCode === 'country:argentina'
          && place.cityName === 'Ushuaia'
        )),
      },
    ];

    let failed = 0;
    cases.forEach((testCase) => {
      const result = parseTravelImportText(testCase.text);
      const ok = Boolean(testCase.check(result));
      if (!ok) failed += 1;
      console.log(`${ok ? 'PASS' : 'FAIL'}  ${testCase.name}`);
    });

    if (failed) process.exitCode = 1;
  }
} finally {
  fs.rmSync(output, { recursive: true, force: true });
}
