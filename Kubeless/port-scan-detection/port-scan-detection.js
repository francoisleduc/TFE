'use strict';

const mongodb = require('mongodb');

const MongoClient = mongodb.MongoClient;
// replicat set URI for mongodb with pod mangod-0 and service named mongondb-service running on port 27017 (default one for mongo)
// replicatset named MainRepSet
const url = 'mongodb://mongod-0.mongodb-service:27017,mongod-1.mongodb-service:27017,mongod-2.mongodb-service:27017/admin?replicaSet=MainRepSet';

module.exports = {
  create: function (event, context) {
      console.log(event.data); // just for myself 
      MongoClient.connect(url, (cerr, client) => {
        if (cerr) {
          console.log("Error when initiating connection to the database primary replicate");
          reject(cerr);
        } else {
          var db = client.db('counters'); // database is called counters in which we also created a collection called counters 
          var myquery = { name: "8080"}; // this port should be read from event.data obviously 
          db.collection('ports').find(myquery).toArray(function (err, result) {
          if (err) 
            throw err;
          console.log(result);
          if (result.length > 0) {
            if (result[0] > 100) { // not sure we should do check on static threshold values for detecting such an attack but it is a start
              console.log("Detection");
              return "Example: Fake port scan detected on 8080";
            }
          }
          client.close();
          });
        }
    });
    return event.data; // just for myself too 
  }
}