'use strict';

const mongodb = require('mongodb');

const MongoClient = mongodb.MongoClient;
const EMPTY_UDP_THRESHOLD = 2000;


// replicat set URI for mongodb with pod mangod-0 and service named mongondb-service running on port 27017 (default one for mongo)
// replicatset named MainRepSet
const url = 'mongodb://mongod-0.mongodb-service:27017,mongod-1.mongodb-service:27017,mongod-2.mongodb-service:27017/admin?replicaSet=MainRepSet';

module.exports = {
  portscan: function (event, context) {
      console.log(event.data); // just for myself 
      MongoClient.connect(url, (cerr, client) => {
        if (cerr) {
           console.log("Error when initiating connection to the database primary replicate");
           reject(cerr);
        } else {
           var db = client.db('counters'); // database is called counters 
           var d = event.data
           const fquery = {"srcip": d["srcip"], "dstip": d["dstip"], "dstport": d["dstport"], "protocol": d["protocol"]};
           const submit = { "srcip": d["srcip"], "dstip": d["dstip"], "dstport": d["dstport"], "protocol": d["protocol"], "udp": d["udp"], "syn": d["syn"], "rst": d["rst"]};
           db.collection('port-stats').find(fquery).toArray(function (err, result) {
           if (err) 
              throw err;
           if(result.length > 0){
              var empty = result[0]["udp"];
              var newval = { $set: {"udp": empty}};
              db.collection('port-stats').updateOne(fquery, newval, function(err, res) {
                if(err)
                  throw err;
              });
              if(newflowsize > EMPTY_UDP_THRESHOLD)
                console.log("Scan detected");
           }
           else {
           //var submit = { "srcip": d["srcip"], "dstip": d["dstip"], "srcport": d["srcport"], "dstport": d["dstport"], "protocol": d["protocol"], "count": d["count"], "size": d["size"]};
           db.collection('port-stats').insertOne(submit, (errInsert) => {
           if (errInsert) {
              console.log("Could not insert in database");
              reject(errInsert);
           }
           });
           }
           });  
        }
    });
    return event.data; // just for myself
  }
}
