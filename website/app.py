from flask import Flask, jsonify, request, render_template
from datetime import datetime, timedelta

app = Flask(__name__)

# Dictionary to store real-time miner data
miner_data = {f'Miner {i}': {'status': 'inactive', 'last_updated': datetime.now()} for i in range(1, 11)}

# Home route to serve the main page
@app.route('/')
def home():
    return render_template('index.html', miner_data=miner_data)

# Endpoint to receive data from ESP32
@app.route('/data', methods=['POST'])
def receive_data():
    try:
        data = request.get_json()
        miner_id = data.get('miner_id')
        print("DATA", data)
        # Check if miner_id is valid
        if miner_id not in miner_data:
            return jsonify({"error": "Invalid miner ID"}), 400

        # Extract sensor values
        methane = data.get('methane')
        carbon_monoxide = data.get('carbon_monoxide')
        temperature = data.get('temperature')
        humidity = data.get('humidity')

        # Update miner data with the latest timestamp
        miner_data[miner_id] = {
            'methane': methane,
            'carbon_monoxide': carbon_monoxide,
            'temperature': temperature,
            'humidity': humidity,
            'last_updated': datetime.now(),  # Track the last update time
            'status': 'alert' if (
                methane > 800 or
                carbon_monoxide > 1500 or
                temperature > 50 or
                humidity > 90
            ) else 'active'
        }

        return jsonify({"message": f"Data received from {miner_id}"}), 200

    except Exception as e:
        return jsonify({"error": f"Error processing request: {str(e)}"}), 400

# Endpoint to get the status of a specific miner
@app.route('/miner-status/<miner_id>', methods=['GET'])
def miner_status(miner_id):
    # Check if miner data exists
    if miner_id in miner_data:
        return render_template('miner_status.html', miner_id=miner_id, **miner_data[miner_id])
    else:
        return jsonify({"error": f"No data available for {miner_id}"}), 404

# Endpoint to fetch real-time data for the dashboard
@app.route('/data/<miner_id>', methods=['GET'])
def get_miner_data(miner_id):
    if miner_id in miner_data:
        return jsonify(miner_data[miner_id])
    else:
        return jsonify({"error": f"No data available for {miner_id}"}), 404
    
@app.route('/miner-data', methods=['GET'])
def get_all_miner_data():
    return jsonify(miner_data)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
