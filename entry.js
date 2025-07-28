const express = require('express');
const mongoose = require('mongoose');
require('dotenv').config();
const app = express();
const path = require('path');
const session=require('express-session')
const authRoutes = require('./router/authRoutes');
const { connectDB } = require('./database/dbConnection');

app.use(express.static(path.join(__dirname, 'public')));
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(session({
  secret:'your-secret-key',
  resave:false,
  saveUninitialized: true,
  cookie:{
    secure:false
  }
}))
app.set('view engine', 'ejs');
app.set('views', './views');
app.listen(process.env.PORT || 3000, () => {
  console.log(`Server is running on port ${process.env.PORT || 3000}`);
  connectDB();
});
app.get('/', (req, res) => {
  res.render('homepage',{title :"guide"});
});
//app.use((req,res,next)=>{
  //if(req.session.user){
    //next();
  //}
  //else{
   // res.redirect('/login');
  //}
//})
app.use('/api',authRoutes);