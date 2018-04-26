var express = require('express');
var router = express.Router();


/* GET home page. */
router.get('/', ensureAuthenticated, function(req, res, next) {
  res.render('index', { title: 'Express' });
});

// router.get('/coffee_test', function(req, res, next) {
//   res.render('coffee_test',{title:'Coffee1'});
// });

// router.get("/coffee_test", function(req, res) {  
//     console.log (req.query);
//     // var response = {message: 'You ordered'};
//     // res.send(response);
//      req.flash('success', 'You ordered ' + req.query.lenght + ' coffee');
//      // res.redirect("/coffee_test");
//      res.redirect('back');
// })

// router.get ('/coffee_test/:lenght/:count',function(req, res, next) {
// 	var coffee_lenght = req.params.lenght;
// 	var coffee_count = req.params.count;
// 	var response = {message: 'You ordered ' + coffee_count + ' coffies with ' + coffee_lenght + ' lenght'};
// 	res.send(response);
// });


function ensureAuthenticated(req, res, next){
	if(req.isAuthenticated()){
		return next();
	}
	res.redirect('/users/login');
}


module.exports = router;
