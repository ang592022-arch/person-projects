# ocrImport

This cloud function is the server-side entry for screenshot OCR.

The client first calls the function with `action: 'prepare'`. The function
returns an `ocr-import/{openid}/` upload prefix for the current caller. The
recognition call rejects file IDs outside that prefix, so one user cannot ask
OCR to read another user's source file. The source is deleted after recognition.

Provider: Tencent Cloud OCR `GeneralAccurateOCR`.

The function downloads the CloudBase source image, calls OCR with a server-side
TC3 signature, and deletes the temporary source in `finally`. It enforces both a
per-openid daily quota and a whole-app monthly budget through transactional
writes to the `ocr_usage` collection.

Configure these cloud-function environment variables:

- `OCR_SECRET_ID` and `OCR_SECRET_KEY`: recommended dedicated sub-account keys.
- `OCR_SESSION_TOKEN`: optional when temporary credentials are used.
- `OCR_ACTION`: optional, defaults to `GeneralAccurateOCR`.
- `OCR_DAILY_LIMIT`: optional, defaults to `30` calls per openid per UTC day.
- `OCR_MONTHLY_LIMIT`: optional, defaults to `900` calls for the whole app per
  UTC month. This conservative ceiling prevents accidental paid overage.
- `OCR_KEEP_SOURCE`: optional; only `true` keeps uploaded OCR screenshots.

If the CloudBase runtime already injects `TENCENTCLOUD_SECRETID`,
`TENCENTCLOUD_SECRETKEY`, and `TENCENTCLOUD_SESSIONTOKEN`, the function uses
those credentials instead. Grant that runtime identity permission to invoke the
configured OCR API. Never put credentials in the mini-program code.

Create the private `ocr_usage` collection before deployment. Only the cloud
function needs access to this collection.

After deployment, open the function's version/configuration panel and set the
execution timeout to at least 20 seconds. The function downloads the source,
calls the OCR API, updates the daily quota, and removes the temporary image in
one invocation; the platform's 3-second default is too short for this workflow.

Expected response shape:

```js
{
  text: 'recognized text',
  fullText: 'recognized text',
}
```
