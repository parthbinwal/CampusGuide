const bcrypt=require('bcrypt');
const {userModel}=require('../models/userMode')


const getLogin=(req, res) => {
  res.render('login', { title: 'Login' });
}




const postLogin=async(req,res)=>{
  const { email, password } = req.body;
  if(!email || !password) {
    return res.status(400).send('All fields are required');
  }
 try {
  const user=await userModel.findOne({email});
  if(!user){
    return res.sendStatus(400).send("account not exist");

  }
  const checkpassword=await bcrypt.compare(password,user.password);
  if(!checkpassword){
    return res.send("password does not match");

  }
  req.session.user=email;
  res.redirect('/api/map');
 } 
catch (error) {
  console.log(error);
  return res.send("server error");
 }
}

const getRegister=(req, res) => {
  res.render('register', { title: 'Register' });
}


const postRegister=async (req, res) => {
  const { username, email, password } = req.body;
  if(!username || !email || !password) {
    return res.status(400).send('All fields are required');
  }
  try {
    const existingUser=await userModel.findOne({ email });
    if(existingUser) {
      return res.status(400).send('User already exists');
    }
    const hashedPassword = await bcrypt.hash(password, 10);
    const newUser = new userModel({ username, email, password: hashedPassword });
    await newUser.save();
  } catch (error) {
    console.error(error);
    return res.status(500).send('Internal Server Error');
  }
  res.redirect('/api/login');
}


const getMap=(req,res)=>{
  if(!req.session.user){
    return res.redirect('/');
  }
  res.render('Map',{ result: null, osm_link: null });
}




module.exports={getLogin,postLogin,getRegister,postRegister,getMap}