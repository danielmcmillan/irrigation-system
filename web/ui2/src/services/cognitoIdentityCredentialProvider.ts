import { CognitoIdentityClient } from "@aws-sdk/client-cognito-identity";
import {
  CognitoIdentityCredentialProvider,
  fromCognitoIdentityPool,
} from "@aws-sdk/credential-provider-cognito-identity";
import { CognitoIdentityTokenProvider } from "./cognitoIdentityTokenProvider";

export interface CognitoIdentityCredentialProviderOptions {
  region: string;
  userPoolId: string;
  identityPoolId: string;
}

export function getCognitoIdentityCredentialProvider(
  identityProvider: CognitoIdentityTokenProvider,
  {
    region,
    userPoolId,
    identityPoolId,
  }: CognitoIdentityCredentialProviderOptions
): CognitoIdentityCredentialProvider {
  const cognitoIdentity = new CognitoIdentityClient({ region });
  const cognitoId = `cognito-idp.${region}.amazonaws.com/${userPoolId}`;
  return async () => {
    const idToken = await identityProvider.getIdToken();
    if (!idToken) {
      throw new Error(
        "Failed to retrieve credentials because user is not authenticated."
      );
    }
    return fromCognitoIdentityPool({
      client: cognitoIdentity,
      identityPoolId,
      logins: {
        [cognitoId]: idToken,
      },
    })();
  };
}
