var express = require('express');
var router = express.Router();

router.get("/", function(req, res) {  
    // console.log(req.query);
    console.log (req.query);
    var response = {message: 'You ordered coffee with ' + req.query.lenght + ' lenght'};
    res.send(response);
})

module.exports = router;