const mongoose=require('mongoose');
const connectDB=async()=>{
  try {
    const conn=mongoose.connect(process.env.MONGO_URI);
    console.log("database connceted")
  } catch (error) {
    console.log('error');
    process.exit(1);
  }
}
module.exports= {connectDB};