export interface CognitoIdentityTokenProviderOptions {
  clientId: string;
  loginDomain: string;
  redirectUri: string;
}

/**
 * Handles authenticating and retrieving identity token from Cognito hosted login service using OAuth code grant.
 */
export class CognitoIdentityTokenProvider {
  private idToken: string | undefined;
  private idTokenExpiryTime: number | undefined;
  private refreshToken: string | undefined;

  constructor(public readonly options: CognitoIdentityTokenProviderOptions) {
    this.restore();
  }

  get available(): boolean {
    return (
      this.idToken !== undefined &&
      this.idTokenExpiryTime !== undefined &&
      this.idTokenExpiryTime > Date.now() + 30000
    );
  }

  async handleAuthentication(params: URLSearchParams): Promise<boolean> {
    const code = params.get("code");
    if (code) {
      try {
        await this.requestToken({ code });
        return true;
      } catch (err) {
        console.error("Failed to handle authentication response", err);
        return false;
      }
    } else {
      console.error("Received invalid authentication response", params);
      return false;
    }
  }

  async getIdToken(): Promise<string | undefined> {
    if (this.available || (await this.refreshTokens())) {
      return this.idToken;
    }
    return undefined;
  }

  logout() {
    localStorage.removeItem("auth");
    this.idToken = undefined;
    this.idTokenExpiryTime = undefined;
    this.refreshToken = undefined;
    this.login(true);
  }

  private async refreshTokens(): Promise<boolean> {
    if (!this.refreshToken) {
      this.login();
      return false;
    }
    try {
      await this.requestToken({
        refreshToken: this.refreshToken,
      });
      return true;
    } catch (err) {
      console.error("Failed to refresh tokens", err);
      if (
        typeof err === "object" &&
        err &&
        "cause" in err &&
        typeof err?.cause === "object" &&
        err.cause &&
        "reauthRequired" in err.cause &&
        err.cause.reauthRequired
      ) {
        this.login();
      }
      return false;
    }
  }

  private login(logout = false) {
    const authorizeParams = new URLSearchParams({
      response_type: "code",
      client_id: this.options.clientId,
      redirect_uri: this.options.redirectUri,
    });
    const endpoint = logout ? "logout" : "oauth2/authorize";
    window.location.href = `https://${this.options.loginDomain}/${endpoint}?${authorizeParams}`;
  }

  private async requestToken(grant: { code: string } | { refreshToken: string }): Promise<void> {
    const body = new URLSearchParams({
      client_id: this.options.clientId,
      redirect_uri: this.options.redirectUri,
    });
    if ("code" in grant) {
      body.append("grant_type", "authorization_code");
      body.append("code", grant.code);
    } else {
      body.append("grant_type", "refresh_token");
      body.append("refresh_token", grant.refreshToken);
    }
    const result = await fetch(`https://${this.options.loginDomain}/oauth2/token`, {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
      },
      body,
    });
    if (result.ok) {
      const tokens: {
        access_token: string;
        expires_in: number;
        id_token: string;
        refresh_token?: string;
        token_type: string;
      } = await result.json();
      this.idToken = tokens.id_token;
      this.idTokenExpiryTime = Date.now() + tokens.expires_in * 1000;
      if (tokens.refresh_token) {
        this.refreshToken = tokens.refresh_token;
      }
      this.persist();
    } else {
      throw new Error(`Token request failed: ${result.status}. ${await result.text()}`, {
        cause: { reauthRequired: result.status === 400 },
      });
    }
  }

  private persist() {
    if (this.refreshToken) {
      localStorage.setItem(
        "auth",
        btoa(
          JSON.stringify({
            i: this.idToken,
            e: this.idTokenExpiryTime,
            r: this.refreshToken,
          })
        )
      );
    }
  }

  private restore() {
    const auth = localStorage.getItem("auth");
    try {
      if (auth) {
        const { i, e, r } = JSON.parse(atob(auth));
        this.idToken = i;
        this.idTokenExpiryTime = e;
        this.refreshToken = r;
      }
    } catch (err) {
      console.warn("Ignoring invalid auth state in local storage");
    }
  }
}
