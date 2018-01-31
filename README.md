# cppurl
Small C++ wrapper for the curl library

## Use

```c++
Cppurl cppurl;

// Enable or disable the SSL verification.
// CURLOPT_SSL_VERIFYPEER - verify the peer's SSL certificate 
// CURLOPT_SSL_VERIFYHOST - verify the certificate's name against host
cppurl.setAvoidSSL(true);

// Enable or disable verbose mode.
cppurl.setVerbose(false);

// Set the user and password.
std::string userpass = "usr:password";
cppurl.setUserAndPwd(userpass);

// Set the content type of the post data.
// Options: Cppurl::json or Cppurl::text
cppurl.setContentType(Cppurl::json);

// POST:
string response;
int http = cppurl.post(url, json, response);
if (http == 400)
{
        terror("Bad request!");
        exit(1);
}

// GET:
string response;
int http = cppurl.get(url, response);

```

## Compilation

Include the "cppurl.hpp" header, and link with:

-lcurl
