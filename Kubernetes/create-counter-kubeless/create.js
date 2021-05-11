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
          var db = client.db('counters'); // database is called counters 
          var myquery = { name: "8080", value: "130012"};
          db.collection('ports').insertOne(myquery, (errInsert) => {
            if (errInsert) {
              console.log("Could not insert in database");
              reject(errInsert);
            } else {
              //resolve(JSON.stringify(data));
              client.close();
            }
          });
        }
    });
    return event.data; // just for myself 
  }
}