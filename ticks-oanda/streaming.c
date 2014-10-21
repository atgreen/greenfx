#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <curl/curl.h>

#include "config.h"
 
// ------ Change these lines ------
char *domain = OANDA_STREAM_DOMAIN;
char *accessToken = OANDA_ACCESS_TOKEN;
char *accounts = OANDA_ACCOUNT_ID;
// --------------------------------

/**********************************
* The domain variable should be:
* 
* For Sandbox    -> http://stream-sandbox.oanda.com
* For fxPractice -> https://stream-fxpractice.oanda.com
* For fxTrade    -> https://stream-fxtrade.oanda.com
**********************************/
 
static size_t httpCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  syslog (LOG_INFO, "%s", (char*)contents); 
  return size * nmemb;
}
 
int main(void)
{
    CURL *curl_handle;
    CURLcode res;
    char authHeader[100];
    char url[100];
    struct curl_slist *chunk = NULL;

    openlog ("oanda-ticks", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    syslog (LOG_NOTICE, "Program started by User %d", getuid());

    if (snprintf(authHeader, 100, "Authorization: Bearer %s", accessToken) >= 100)
        exit(1);
    if (snprintf(url, 100, "%s/v1/prices?accountIds=%s&instruments=USD_CAD", domain, accounts) >= 100)
        exit(1);

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */ 
    curl_handle = curl_easy_init();

    /* specify URL to get */ 
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, httpCallback);

    chunk = curl_slist_append(chunk, authHeader);

    /* use custom headers */
    res = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);

    /* get it! */ 
    res = curl_easy_perform(curl_handle);

    /* check for errors */ 
    if(res != CURLE_OK) 
    {
      syslog (LOG_ERR, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }
    else 
    {
      // Stream has been disconnected, server might be down, or too many connections 
      syslog (LOG_NOTICE, "disconnected from stream");
    }

    /* cleanup curl stuff */ 
    curl_easy_cleanup(curl_handle);

    /* we're done with libcurl, so clean it up */ 
    curl_global_cleanup();

    syslog (LOG_NOTICE, "Program ended");

    return 0;
}

