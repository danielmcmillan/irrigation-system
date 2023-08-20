import { Sha256 } from "@aws-crypto/sha256-browser";
import { HttpRequest } from "@smithy/protocol-http";
import { SignatureV4 } from "@smithy/signature-v4";
import {
  CognitoIdentityCredentialProviderOptions,
  getCognitoIdentityCredentialProvider,
} from "./cognitoIdentityCredentialProvider";
import { CognitoIdentityTokenProvider } from "./cognitoIdentityTokenProvider";

export class ApiRequestSigner {
  private executeApiSigner: SignatureV4;

  constructor(
    public readonly options: CognitoIdentityCredentialProviderOptions & {
      identityTokenProvider: CognitoIdentityTokenProvider;
      wsApiUrl: string;
    }
  ) {
    const credentialProvider = getCognitoIdentityCredentialProvider(
      options.identityTokenProvider,
      options
    );
    this.executeApiSigner = new SignatureV4({
      credentials: credentialProvider,
      region: options.region,
      service: "execute-api",
      sha256: Sha256,
      applyChecksum: false,
      uriEscapePath: true,
    });
  }

  /** Gets a signed connection URL for the WebSocket API. */
  async getWsApiUrl(): Promise<string> {
    const apiUrl = new URL(this.options.wsApiUrl);
    const request = new HttpRequest({
      method: "GET",
      hostname: apiUrl.host,
      path: apiUrl.pathname,
      headers: {
        host: apiUrl.hostname,
      },
    });
    const signed = await this.executeApiSigner.presign(request);
    for (const [name, value] of Object.entries(signed.query!)) {
      apiUrl.searchParams.set(name, value as string);
    }
    return apiUrl.toString();
  }
}
