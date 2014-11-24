// --------------------------------------------------------------------------
//  _____                    ________   __
// |  __ \                   |  ___\ \ / /
// | |  \/_ __ ___  ___ _ __ | |_   \ V /          Open Source Tools for
// | | __| '__/ _ \/ _ \ '_ \|  _|  /   \            Automated Algorithmic
// | |_\ \ | |  __/  __/ | | | |   / /^\ \             Currency Trading
//  \____/_|  \___|\___|_| |_\_|   \/   \/
//
// --------------------------------------------------------------------------

// Copyright (C) 2014  Anthony Green <green@spindazzle.org>
// Distrubuted under the terms of the GPL v3 or later.

// This progam pulls ticks from the A-MQ message bus and records them
// in our InfluxDB time series database.

#include <cstdlib>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <influxdb.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>

#include <activemq/library/ActiveMQCPP.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

#include <json/json.h>

using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace activemq;
using namespace cms;
using namespace std;

#include "config.h"
 
static const char *domain = OANDA_STREAM_DOMAIN;
static const char *accessToken = OANDA_ACCESS_TOKEN;
static const char *accounts = OANDA_ACCOUNT_ID;

static influxdb_client *idbc;

class TickListener : public ExceptionListener,
		     public MessageListener,
  		     public Runnable {

private:
  Session *session;
  Connection *connection;
  Destination *destination;
  MessageConsumer *consumer;

  string brokerURI;

public:
  TickListener () :
    brokerURI("tcp://" ACTIVEMQ_BROKER_IP ":61616") {
  }

  virtual void run () {
    try {

      // Create a ConnectionFactory
      std::auto_ptr<ConnectionFactory> 
	connectionFactory(ConnectionFactory::createCMSConnectionFactory(brokerURI));
      
      // Create a Connection
      connection = connectionFactory->createConnection(ACTIVEMQ_USERNAME,
						       ACTIVEMQ_PASSWORD);
      connection->start();
      connection->setExceptionListener(this);
      
      session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
      destination = session->createTopic("OANDA.TICK");
      
      consumer = session->createConsumer(destination);
      consumer->setMessageListener(this);

      // Sleep forever
      while (true)
	sleep(1000);
      
    } catch (CMSException& ex) {
      
      syslog (LOG_ERR, ex.getStackTraceString().c_str());
      exit (1);
      
    }
  }

  virtual void onMessage(const Message *msg)
  {
    json_object *jobj = json_tokener_parse (dynamic_cast<const TextMessage*>(msg)->getText().c_str());

    jobj = json_object_object_get(jobj, "tick");
    if (jobj != NULL)
      {
	json_object *bid = json_object_object_get(jobj, "bid");
	json_object *ask = json_object_object_get(jobj, "ask");
	influxdb_series *series = influxdb_series_create ("ticks", NULL);
	const char **tab1 = (const char **) malloc(sizeof (char *) * 2);
	influxdb_series_add_colums(series, "bid");
	influxdb_series_add_colums(series, "ask");
	tab1[0] = json_object_get_string(bid);
	tab1[1] = json_object_get_string(ask);
	influxdb_series_add_points(series, (char **) tab1);
	influxdb_write_serie(idbc, series);
	influxdb_series_free(series, NULL);
      }
  }

  virtual void onException(const CMSException& ex)
  {
    syslog (LOG_ERR, ex.getStackTraceString().c_str());
    exit (1);
  }
};


void ensure_db (influxdb_client *idbc)
{
  char **dblist;
  size_t nb = influxdb_get_database_list (idbc, &dblist);
  int find = 0;
  size_t i;

  // It's OK to leak the memory for the db names.  We just do this
  // once.
  for (i = 0; i < nb; i++)
    if (!strcmp(dblist[i], "USD_CAD"))
      return;
  
  // Not found.  We need to create it.
  influxdb_create_database (idbc, "USD_CAD");
}

int main()
{
  openlog ("oanda-ticks", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  syslog (LOG_NOTICE, "Program started by User %d", getuid());

  idbc = influxdb_client_new (INFLUXDB_IP ":8086",
			      "root", "root",
			      "USD_CAD",
			      0);

  ensure_db (idbc);

  activemq::library::ActiveMQCPP::initializeLibrary();

  TickListener tick_listener;
  Thread listener_thread(&tick_listener);
  listener_thread.start();
  listener_thread.join();

  syslog (LOG_NOTICE, "Program ended");

  return 0;
}

