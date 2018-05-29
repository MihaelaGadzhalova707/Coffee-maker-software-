var express = require('express');
var router = express.Router();


/* GET home page. */
router.get('/', ensureAuthenticated, function(req, res, next) {
  res.render('index', { title: 'Express' });
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
})

function ensureAuthenticated(req, res, next){
	if(req.isAuthenticated()){
		return next();
	}
	res.redirect('/users/login');
}

module.exports = router;
