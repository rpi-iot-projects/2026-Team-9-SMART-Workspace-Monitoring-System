// Change the header color and mode text
function changeNavMode(newClass) {
    const nav = document.getElementById('main-nav');
    const modeBadge = document.getElementById('modeText');

    //Remove custom classes first so they don't stack
    nav.classList.remove('bg-eating', 'bg-study', 'bg-rgb', 'bg-dark');

    // Add the new class
    nav.classList.add(newClass);

    if(newClass === 'bg-eating'){
        modeBadge.innerText = "Eating Mode";
        currentMode = "Eating";
    }
    else if(newClass === 'bg-study'){
        modeBadge.innerText = "Studying Mode";
        currentMode = "Studying";
    }   
    else if(newClass === 'bg-rgb'){
        modeBadge.innerText = "Gaming Mode";
        currentMode = "Gaming";
    } 
    
}

// Change alerts based on threshold
function updatePostureStatus(mode, leftWeight, rightWeight, distance) {
    const alertBox = document.getElementById("statusAlert");

    let isGood = true;
    let message = "Posture is optimal";

    const avgWeight = (leftWeight + rightWeight) / 2;

    switch (mode) {
        case "Eating":
            if (distance > 40 && avgWeight < 400) {
                isGood = false;
                message = "No desk interaction detected: stop scrolling";
            } 
            else if (distance < 20 && avgWeight > 3000) {
                isGood = false;
                message = "Excessive leaning detected: please sit up";
            }
            break;

        case "Gaming":
            if (distance < 12) {
                isGood = false;
                message = "Too close to the screen: fix your posture";
            }
            break;

        case "Studying":
            if (avgWeight < 490) {
                isGood = false;
                message = "No desk interaction detected: stop scrolling";
            }
            break;

        default:
            isGood = true;
            message = "System idle.";
    }

    // Update UI
    alertBox.classList.remove("alert-success", "alert-danger");

    if (isGood) {
        alertBox.classList.add("alert-success");
        alertBox.innerHTML = `<strong>${message}</strong> `;
    } else {
        alertBox.classList.add("alert-danger");
        alertBox.innerHTML = `<strong>${message}</strong> `;
    }
}

const MAX_POINTS = 20;
let dataQueue = [];

// Charts
const distanceCtx = document.getElementById('distanceChart').getContext('2d');
const weightCtx = document.getElementById('weightChart').getContext('2d');

const distanceChart = new Chart(distanceCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Distance (cm)',
            data: [],
            borderWidth: 2,
            fill: false
        }]
    },
    options: {
        animation: false
    }
});

const weightChart = new Chart(weightCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [
            {
                label: 'Left Weight (g)',
                data: [],
                borderWidth: 2,
                fill: false
            },
            {
                label: 'Right Weight (g)',
                data: [],
                borderWidth: 2,
                fill: false
            }
        ]
    },
    options: {
        animation: false
    }
});

// SSE connection
const source = new EventSource("/sse/");
let currentState = {
    distance: null,
    left_weight: null,
    right_weight: null
};
let lastRFID = null;
let stateReady = {
    distance: false,
    weight: false
};
let currentMode = "Idle";

function isStateReady() {
    return stateReady.distance && stateReady.weight;
}

source.onmessage = function(event) {
    const newData = JSON.parse(event.data);

    // Distance stream
    newData.distances.forEach(d => {
        dataQueue.push({
            type: "distance",
            value: d.value,
            time: d.time
        });
    });

    // Weight stream
    newData.weights.forEach(w => {
        dataQueue.push({
            type: "weight",
            left: w.left,
            right: w.right,
            time: w.time
        });
    });

    //RFID Stream
    if (newData.rfids) {
        newData.rfids.forEach(r => {
            dataQueue.push({
                type: "rfid",
                value: r.value,
                time: r.time
            });
        });
    }
};

setInterval(() => {
    if (dataQueue.length === 0) return;
    const point = dataQueue.shift();

    if (point.type === "distance") {
        currentState.distance = point.value;
        stateReady.distance = true;
    }
    if (point.type === "weight") {
        currentState.left_weight = point.left;
        currentState.right_weight = point.right;
        stateReady.weight = true;
    }

    if (point.type === "rfid") {
        // Prevent spamming same mode
        if (point.value !== lastRFID) {
            lastRFID = point.value;

            // Map RFID → mode
            if (point.value === "Eating") {
                changeNavMode("bg-eating");
            } 
            else if (point.value === "Studying") {
                changeNavMode("bg-study");
            } 
            else if (point.value === "Gaming") {
                changeNavMode("bg-rgb");
            }

            console.log("RFID mode changed:", point.value);
        }
    }

    if (isStateReady() && currentMode) {
        updatePostureStatus(
            currentMode,
            currentState.left_weight,
            currentState.right_weight,
            currentState.distance
        );
    }

    // Distance chart
    if (currentState.distance !== null) {
        const time = new Date().toLocaleTimeString();
        distanceChart.data.labels.push(time);
        distanceChart.data.datasets[0].data.push(currentState.distance);
        if (distanceChart.data.labels.length > MAX_POINTS) {
            distanceChart.data.labels.shift();
            distanceChart.data.datasets[0].data.shift();
        }
        distanceChart.update('none');
    }
    // Weight chart
    if (currentState.left_weight !== null && currentState.right_weight !== null) {
        weightChart.data.labels.push(point.time);
        weightChart.data.datasets[0].data.push(currentState.left_weight);
        weightChart.data.datasets[1].data.push(currentState.right_weight);
        if (weightChart.data.labels.length > MAX_POINTS) {
            weightChart.data.labels.shift();
            weightChart.data.datasets[0].data.shift();
            weightChart.data.datasets[1].data.shift();
        }
        weightChart.update('none');
    }
}, 200);
