module.exports = {
    //...
    output: {
        //...
        chunkFilename: () => { return "game-libraries.[contenthash].js" }
    }
};