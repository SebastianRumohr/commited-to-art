# commited-to-art

## src/secrets.h
``` c
#ifndef _SECRETS_H
#define _SECRETS_H

struct
{
    const char *ssid = "ssid";
    const char *password = "password";
} WiFiConfig;

struct
{
    const char *oAuthToken = "oAuthToken";
    const char *repository = "organisation/repository";
    const char *branch = "branch";
} GitHub;

#endif
```