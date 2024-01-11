const socket = io();

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

function push_camera_state () {
    socket.emit("camera-angles", shared_data.camera_state);
}

function render () {
    const lock_border = document.getElementById("lock_border");
    if (shared_data.is_locked) {
        lock_border.display = 'block';
        lock_border.style.border = `20px solid ${shared_data.lock_color}`;
    } else {
        lock_border.display = 'none';
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
    if (!shared_data.sockets.find(o => o.id == data.id)) {
        shared_data.sockets.push(data);
        render();
    }
});

socket.on("socket-disconnected", (data) => {
    shared_data.sockets = shared_data.sockets.filter(o => o.id != data.id);
    render();
});

socket.on("camera-angles", (data) => {
    shared_data.color = data.color;
    shared_data.camera_state.theta = data.camera_state.theta;
    shared_data.camera_state.phi = data.camera_state.phi;
    Module._update_camera_state(
        shared_data.camera_state.theta,
        shared_data.camera_state.phi
    );
});

socket.on("update-lock", (data) => {
    shared_data.lock_color = data.color;
    shared_data.is_locked = true;
    Module._lock();
    render();
});

socket.on("free-lock", (data) => {
    shared_data.lock_color = null;
    shared_data.is_locked = false;
    Module._free_lock();
    render();
});
