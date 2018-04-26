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
    // var response = {message: 'You ordered'};
    // res.send(response);
     req.flash('success', 'You ordered ' + req.query.lenght + ' coffee');
     // res.redirect("/coffee_test");
     res.redirect('/');
})

module.exports = router;