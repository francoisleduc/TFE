'use strict';

const mongodb = require('mongodb');

const MongoClient = mongodb.MongoClient;
const FLOW_HEAVY_MAX_BOUNDARY = 20000000;


// replicat set URI for mongodb with pod mangod-0 and service named mongondb-service running on port 27017 (default one for mongo)
// replicatset named MainRepSet
const url = 'mongodb://mongod-0.mongodb-service:27017,mongod-1.mongodb-service:27017,mongod-2.mongodb-service:27017/admin?replicaSet=MainRepSet';

module.exports = {
  heavyhitter: function (event, context) {
      console.log(event.data); // just for myself 
      MongoClient.connect(url, (cerr, client) => {
        if (cerr) {
           console.log("Error when initiating connection to the database primary replicate");
           reject(cerr);
        } else {
           var db = client.db('counters'); // database is called counters 
	       var d = event.data
           const fquery = {"srcip": d["srcip"], "dstip": d["dstip"], "srcport": d["srcport"], "dstport": d["dstport"], "protocol": d["protocol"]};
           const submit = { "srcip": d["srcip"], "dstip": d["dstip"], "srcport": d["srcport"], "dstport": d["dstport"], "protocol": d["protocol"], "count": d["count"], "size": d["size"]};
           db.collection('flows').find(fquery).toArray(function (err, result) {
           if (err) 
              throw err;
           if(result.length > 0){
              var newflowsize = result[0]["count"];
              var newval = { $set: {"count": newflowsize}};
              db.collection('flows').updateOne(fquery, newval, function(err, res) {
                if(err)
                  throw err;
              });
              if(newflowsize > FLOW_HEAVY_MAX_BOUNDARY)
                console.log("Heavy hitter detected");
           }
           else {
           //var submit = { "srcip": d["srcip"], "dstip": d["dstip"], "srcport": d["srcport"], "dstport": d["dstport"], "protocol": d["protocol"], "count": d["count"], "size": d["size"]};
           db.collection('flows').insertOne(submit, (errInsert) => {
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
