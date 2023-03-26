import { Amplify } from "aws-amplify";
import { AWSIoTProvider } from "./vendor/PubSub/AWSIotProvider";

export interface AmplifyConfig {
  region: string;
  identityPoolId: string;
  mqttEndpoint: string;
}

export function configureAmplify(config: AmplifyConfig): string {
  Amplify.configure({
    Auth: {
      identityPoolId: config.identityPoolId,
      region: config.region,

      // OPTIONAL - Amazon Cognito User Pool ID
      // userPoolId: 'XX-XXXX-X_abcd1234',

      // OPTIONAL - Amazon Cognito Web Client ID (26-char alphanumeric string)
      // userPoolWebClientId: 'a1b2c3d4e5f6g7h8i9j0k1l2m3',

      mandatorySignIn: false,

      // OPTIONAL - This is used when autoSignIn is enabled for Auth.signUp
      // 'code' is used for Auth.confirmSignUp, 'link' is used for email link verification
      // signUpVerificationMethod: 'code', // 'code' | 'link'

      // OPTIONAL - Configuration for cookie storage
      // Note: if the secure flag is set to true, then the cookie transmission requires a secure protocol
      // cookieStorage: {
      // // REQUIRED - Cookie domain (only required if cookieStorage is provided)
      //     domain: '.yourdomain.com',
      // // OPTIONAL - Cookie path
      //     path: '/',
      // // OPTIONAL - Cookie expiration in days
      //     expires: 365,
      // // OPTIONAL - See: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie/SameSite
      //     sameSite: "strict" | "lax",
      // // OPTIONAL - Cookie secure flag
      // // Either true or false, indicating if the cookie transmission requires a secure protocol (https).
      //     secure: true
      // },

      // OPTIONAL - customized storage object
      // storage: MyStorage,

      // OPTIONAL - Manually set the authentication flow type. Default is 'USER_SRP_AUTH'
      // authenticationFlowType: 'USER_PASSWORD_AUTH',

      // OPTIONAL - Manually set key value pairs that can be passed to Cognito Lambda Triggers
      // clientMetadata: { myCustomKey: 'myCustomValue' },

      // OPTIONAL - Hosted UI configuration
      // oauth: {
      //     domain: 'your_cognito_domain',
      //     scope: ['phone', 'email', 'profile', 'openid', 'aws.cognito.signin.user.admin'],
      //     redirectSignIn: 'http://localhost:3000/',
      //     redirectSignOut: 'http://localhost:3000/',
      //     responseType: 'code' // or 'token', note that REFRESH token will only be generated when the responseType is code
      // }
    },
  });

  const clientId = (() => {
    const idArr = new Uint8Array(6);
    crypto.getRandomValues(idArr);
    let idBin = "";
    for (var i = 0; i < idArr.byteLength; ++i) {
      idBin += String.fromCharCode(idArr[i]);
    }
    return window.btoa(idBin).replaceAll("/", "_").replaceAll("+", "-");
  })();

  Amplify.addPluggable(
    new AWSIoTProvider({
      aws_pubsub_region: config.region,
      aws_pubsub_endpoint: config.mqttEndpoint,
      clientId,
    })
  );

  console.log(`Configured with client id "${clientId}"`);
  return clientId;
}
