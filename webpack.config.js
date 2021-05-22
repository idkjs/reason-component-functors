
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const outputDir = path.join(__dirname, 'build');
const isProd = process.env.NODE_ENV === 'production';

module.exports = {
  entry: './src/Index.bs.js',
  mode: isProd ? 'production' : 'development',
    plugins: [
    new HtmlWebpackPlugin({
      title: 'MyComponents',
      template: './src/index.html'
    })
  ],
  output: {
    path: outputDir,
    filename: '[name].bundle.js',
  },
};
