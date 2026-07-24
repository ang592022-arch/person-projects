# Security policy

Please do not open a public issue containing credentials, OpenIDs, travel
records, screenshots, cloud file IDs or other personal data.

For a private deployment:

- keep `AppSecret`, OCR credentials and service keys in server-side environment
  variables or managed runtime identities;
- keep `miniprogram/config/env.ts` out of source control;
- use creator-only database permissions and verify access with two accounts;
- configure OCR quotas and delete temporary source images after recognition;
- rotate any credential immediately if it is committed or shared.

Security reports can be sent through GitHub private vulnerability reporting
when it is enabled for the repository.
