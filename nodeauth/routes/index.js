var express = require('express');
var router = express.Router();

var alarmEvent1 = 0;
/* GET home page. */


router.get('/', ensureAuthenticated, function(req, res, next) {


   console.log(alarmEvent1);
  res.render('index', { title: 'Express' });


 processFile('../mosquitto.log');
});


router.get("/on_off", function(req, res) {  
  
    console.log (req.query);
    console.log(req.query.action);
    const { execSync } = require('child_process');
    // stderr is sent to stdout of parent process
   // you can set options.stdio if you want it to go elsewhere
     let stdout1 = execSync('/usr/bin/mosquitto_pub -t machine -m "' + req.query.action + '"');
     req.flash('success', 'Success');
     res.redirect('back');

});



function processFile(inputFile) {

var fs = require('fs'); // file system module

fs.readFile(inputFile, 'utf-8', function(err, data) {
    if (err) throw err;

    var lines = data.trim().split('\n');
    var lastLine = lines.slice(-2)[0];

    var fields = lastLine.split(':');
    var value = fields.slice(-1)[0].replace('file:\\\\', '');

      if (value!= null ) {
           if(value == 10 || value == 11 || value == 20 ||  value == 21)
             alarmEvent1 = value;

      }
	console.log(value);
});

}


function ensureAuthenticated(req, res, next){
	if(req.isAuthenticated()){
		return next();
	}
	res.redirect('/users/login');
}

module.exports = router;
