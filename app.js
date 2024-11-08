const path = require("path");

const express = require("express");
const app = express();
const server = require("http").createServer(app);
const io = require("socket.io")(server);

function getRandomColor () {
    const colors = ["red", "green", "blue", "yellow", "orange"];
    const randomIndex = Math.floor(Math.random() * colors.length);
    return colors[randomIndex];
}

const AppConfig = {
    locker: null,
    camera_state: {
        fov: 45.0,
        theta: -74.42,
        phi: 34.11
    }
}

io.on("connection", socket => {
    socket.color = getRandomColor();

    const sockets = [];
    for (let s of io.sockets.sockets.values()) {
        sockets.push(s);
    }

    io.emit("socket-connected", sockets.map(o => ({color: o.color, id: o.id})));
    socket.emit("camera-angles", {
        camera_state: AppConfig.camera_state
    });

    socket.on("camera-angles", (data) => {
        if (AppConfig.locker != socket.id)
            return;

        AppConfig.camera_state.fov = data.fov;
        AppConfig.camera_state.theta = data.theta;
        AppConfig.camera_state.phi = data.phi;

        socket.broadcast.emit("camera-angles", {
            color: socket.color,
            camera_state: AppConfig.camera_state
        });
    });

    socket.on("acquire-lock", () => {
       if (AppConfig.locker != null)
            return;

        AppConfig.locker = socket.id;
        socket.broadcast.emit("update-lock", {
            color: socket.color
        });
    });

    socket.on("free-lock", () => {
        if (AppConfig.locker != socket.id)
            return;

        AppConfig.locker = null;
        socket.broadcast.emit("free-lock", {});
    });

    socket.on("disconnect", () => {
        if (AppConfig.locker == socket.id)
            AppConfig.locker = null;
        socket.broadcast.emit("socket-disconnected", {id: socket.id});
    });
});

const port = process.env.PORT || 8080;

app.get("/", function (req, res) {
    res.sendFile(path.resolve(__dirname, "public/index.backup.html"));
});

app.use(express.static("public"));

server.listen(port, function () {
    console.log(`Listening on port ${port}`);
});
