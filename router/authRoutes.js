const express = require('express');
const { getLogin, postLogin, getRegister, postRegister, getMap } = require('../controller/authController');
const authRouter= express.Router();



authRouter.get('/login',getLogin );
authRouter.post('/login',postLogin);
authRouter.get('/register',getRegister);
authRouter.post('/register',postRegister);
authRouter.get('/map',getMap);
module.exports = authRouter;
