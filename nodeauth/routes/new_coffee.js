var express = require('express');
var router = express.Router();

router.get('/', function(req, res, next) {
  res.render('new_coffee',{title:'Members'});
});

router.get('/order', function(req, res, next) {
  res.render('order',{title:'Coffee1'});
});
// router.get("/coffee_test", function(req, res) {  
//     // console.log(req.query);
//     console.log (req.query);
//     var response = {message: 'You ordered coffee with ' + req.query.lenght + ' lenght'};
//     res.send(response);
// })

router.get("/coffee_test", function(req, res) {  
    console.log (req.query);

  const { execSync } = require('child_process');
  /* stderr is sent to stdout of parent process
   you can set options.stdio if you want it to go elsewhere
   */
   let stdoutCoffeeLenght = execSync('/usr/bin/mosquitto_pub -t machine -m "set:1:' + req.query.lenght + '"');
   let stdoutCoffeeConcentration = execSync('/usr/bin/mosquitto_pub -t machine -m "set:2:' + req.query.concentration + '"');
   let stdoutCupsNumber = execSync('/usr/bin/mosquitto_pub -t machine -m "' + req.query.count + '"');
   

    // req.flash('success', 'You ordered ' + req.query.count + ' coffee: ');
  
     res.redirect('/');
})
module.exports = router;