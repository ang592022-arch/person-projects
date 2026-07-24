const fs = require('fs');
const path = require('path');

const root = path.resolve(__dirname, '..');
const failures = [];

function fail(message) {
  failures.push(message);
}

function read(relativePath) {
  return fs.readFileSync(path.join(root, relativePath), 'utf8');
}

function walk(directory) {
  return fs.readdirSync(directory, { withFileTypes: true }).flatMap((entry) => {
    if (['.git', 'node_modules'].includes(entry.name)) return [];
    const absolute = path.join(directory, entry.name);
    return entry.isDirectory() ? walk(absolute) : [absolute];
  });
}

const forbiddenPaths = [
  'miniprogram/packages/map/data/geojson/province-cities',
  'docs/geojson-archive',
];

forbiddenPaths.forEach((relativePath) => {
  if (fs.existsSync(path.join(root, relativePath))) {
    fail(`Private or unverified path is present: ${relativePath}`);
  }
});

const localEnvPath = path.join(root, 'miniprogram/config/env.ts');
if (fs.existsSync(localEnvPath)) {
  const localEnv = fs.readFileSync(localEnvPath, 'utf8');
  const exampleEnv = read('miniprogram/config/env.example.ts');
  if (localEnv !== exampleEnv) {
    fail('Local env.ts differs from the public example; do not commit or package it');
  }
}

const chinaBoundary = read('miniprogram/data/geojson/china-provinces.ts');
if (!chinaBoundary.includes('features: []') || chinaBoundary.includes('compactFeatures')) {
  fail('China province boundary placeholder contains packaged coordinates');
}

const textExtensions = new Set(['.js', '.json', '.md', '.ts', '.wxml', '.wxss', '.yml', '.yaml']);
const secretPatterns = [
  /AKID[A-Za-z0-9]{10,}/,
  /-----BEGIN (?:RSA |OPENSSH )?PRIVATE KEY-----/,
  /cloud1-[A-Za-z0-9]+/,
  /wx[0-9a-f]{16}/,
  /(?:OCR_SECRET_KEY|TENCENTCLOUD_SECRETKEY)\s*[:=]\s*['"][^'"]{8,}['"]/, 
  /(?:appsecret|app_secret)\s*[:=]\s*['"][^'"]{8,}['"]/i,
];

walk(root).forEach((filePath) => {
  if (!textExtensions.has(path.extname(filePath).toLowerCase())) return;
  const source = fs.readFileSync(filePath, 'utf8');
  secretPatterns.forEach((pattern) => {
    if (pattern.test(source)) {
      fail(`Sensitive value pattern found in ${path.relative(root, filePath)}`);
    }
  });
});

['LICENSE', 'THIRD_PARTY_NOTICES.md', 'README.md'].forEach((relativePath) => {
  if (!fs.existsSync(path.join(root, relativePath))) fail(`Missing ${relativePath}`);
});

if (failures.length) {
  failures.forEach((message) => console.error(`FAIL: ${message}`));
  process.exit(1);
}

console.log('Open-source boundary and credential checks passed.');
