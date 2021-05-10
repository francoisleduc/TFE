'use strict';

const mongodb = require('mongodb');

const MongoClient = mongodb.MongoClient;
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
          var myquery = { name: "new-counter-useless", value: "410"};
          db.collection('counters').insertOne(myquery, (errInsert) => {
            if (errInsert) {
              console.log("Could not insert in database");
              reject(errInsert);
            } else {
              resolve(JSON.stringify(data));
              client.close();
            }
          });
        }
    });
    return event.data;
  }
}