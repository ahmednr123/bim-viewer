const socket = io({ autoConnect: false });

setTimeout(function () {
    document.getElementById("loading-screen").style.display = "none";
    socket.connect();
}, 6000);

const shared_data = {
    color: null,
    sockets: [],
    is_locked: false,
    lock_color: null,
    camera_state: {
        fov: 45,
        theta: -74.42,
        phi: 34.11
    }
};

function acquire_lock () {
    socket.emit("acquire-lock", {});
}

function free_socket_lock () {
    socket.emit("free-lock", {});
}

function push_camera_state () {
    console.log("Pushing camera state to server");
    socket.emit("camera-angles", shared_data.camera_state);
}

function render () {
    const lock_border_left = document.getElementById("lock_border_left");
    const lock_border_top = document.getElementById("lock_border_top");
    const lock_border_right = document.getElementById("lock_border_right");
    const lock_border_bottom = document.getElementById("lock_border_bottom");

    function show_lock_borders (color) {
        lock_border_left.style.display = 'block';
        lock_border_left.style.background = `${color}`;
        lock_border_top.style.display = 'block';
        lock_border_top.style.background = `${color}`;
        lock_border_right.style.display = 'block';
        lock_border_right.style.background = `${color}`;
        lock_border_bottom.style.display = 'block';
        lock_border_bottom.style.background = `${color}`;
    }
    function hide_lock_borders () {
        lock_border_left.style.display = 'none';
        lock_border_top.style.display = 'none';
        lock_border_right.style.display = 'none';
        lock_border_bottom.style.display = 'none';
    }

    if (shared_data.is_locked) {
        show_lock_borders(shared_data.lock_color);
    } else {
        hide_lock_borders();
    }

    const sockets_div = document.getElementById("sockets_div");
    sockets_div.innerHTML = shared_data.sockets.map(s =>
        `<div class="socket-div">
            <div class="socket-color color-${s.color}"></div>
            <div class="socket-id">${s.id}</div>
        </div>`
    ).join("");
}

socket.on("socket-connected", (data) => {
    console.log(`connected: ${JSON.stringify(data)}`);
    for (let user of data) {
        if (!shared_data.sockets.find(o => o.id == user.id)) {
            console.log(JSON.stringify(user));
            shared_data.sockets.push(user);
            render();
        }
    }
});

socket.on("socket-disconnected", (data) => {
    shared_data.sockets = shared_data.sockets.filter(o => o.id != data.id);
    render();
});

socket.on("camera-angles", (data) => {
    shared_data.color = data.color;
    shared_data.camera_state.fov = data.camera_state.fov;
    shared_data.camera_state.theta = data.camera_state.theta;
    shared_data.camera_state.phi = data.camera_state.phi;
    console.log(`fov: ${shared_data.camera_state.fov}, theta: ${shared_data.camera_state.theta}, phi: ${shared_data.camera_state.phi}`);
    Module._update_camera_state(
        shared_data.camera_state.fov,
        shared_data.camera_state.theta,
        shared_data.camera_state.phi
    );
});

socket.on("update-lock", (data) => {
    shared_data.lock_color = data.color;
    shared_data.is_locked = true;
    console.log(`color: ${data.color}`);
    Module._lock();
    render();
});

socket.on("free-lock", (data) => {
    shared_data.lock_color = null;
    shared_data.is_locked = false;
    Module._free_lock();
    render();
});
