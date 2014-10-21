// --------------------------------------------------------------------------
//  _____                    ________   __
// |  __ \                   |  ___\ \ / /
// | |  \/_ __ ___  ___ _ __ | |_   \ V /          Open Source Tools for
// | | __| '__/ _ \/ _ \ '_ \|  _|  /   \           Algorithmic Currency
// | |_\ \ | |  __/  __/ | | | |   / /^\ \           Exchange Trading
//  \____/_|  \___|\___|_| |_\_|   \/   \/
//
// --------------------------------------------------------------------------

// Copyright (C) 2014  Anthony Green <green@spindazzle.org>
// Distrubuted under the terms of the GPL v3 or later.

// This progam is responsible for publishing OANDA currency exchange ticks
// through an ActiveMQ message broker.

#include <cstdlib>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <curl/curl.h>

#include <activemq/library/ActiveMQCPP.h>
#include <activemq/core/ActiveMQConnectionFactory.h>

using namespace activemq;
using namespace cms;
using namespace std;

#include "config.h"
 
static const char *domain = OANDA_STREAM_DOMAIN;
static const char *accessToken = OANDA_ACCESS_TOKEN;
static const char *accounts = OANDA_ACCOUNT_ID;

static Session *session;
static MessageProducer *producer;

static size_t httpCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  syslog (LOG_INFO, "%s", (char*)contents); 

  std::auto_ptr<TextMessage> message(session->createTextMessage((char *) contents));
  producer->send(message.get());

  return size * nmemb;
}

int main(void)
{
  CURL *curl_handle;
  CURLcode res;
  char authHeader[100];
  char url[100];
  struct curl_slist *chunk = NULL;

  string brokerURI = "tcp://" ACTIVEMQ_BROKER_IP ":61616";
  Connection *connection;
  Destination *destination;

  openlog ("oanda-ticks", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  syslog (LOG_NOTICE, "Program started by User %d", getuid());

  activemq::library::ActiveMQCPP::initializeLibrary();

  try {
      
    // Create a ConnectionFactory
    std::auto_ptr<ConnectionFactory> 
      connectionFactory(ConnectionFactory::createCMSConnectionFactory(brokerURI));

    // Create a Connection
    connection = connectionFactory->createConnection(ACTIVEMQ_USERNAME,
						     ACTIVEMQ_PASSWORD);
    connection->start();
      
    session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
    destination = session->createTopic("OANDA.TICK");

    producer = session->createProducer(destination);
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

  } catch (CMSException& e) {

    syslog (LOG_ERR, e.getStackTraceString().c_str());
    exit (1);

  }

  if (snprintf(authHeader, 100, 
	       "Authorization: Bearer %s", accessToken) >= 100)
    exit(1);
  if (snprintf(url, 100, 
	       "%s/v1/prices?accountIds=%s&instruments=USD_CAD", 
	       domain, accounts) >= 100)
    exit(1);

  curl_global_init(CURL_GLOBAL_ALL);

  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, httpCallback);
  chunk = curl_slist_append(chunk, authHeader);
  res = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);
  res = curl_easy_perform(curl_handle);

  /* check for errors */ 
  if(res != CURLE_OK) 
    {
      syslog (LOG_ERR, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }
  else 
    {
      syslog (LOG_NOTICE, "disconnected from stream");
    }

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);

  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();

  syslog (LOG_NOTICE, "Program ended");

  return 0;
}

