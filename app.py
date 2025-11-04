from flask import Flask, render_template, request, jsonify
import subprocess
import json
import os

app = Flask(__name__)

# --- Run the compiled C++ program ---
def run_cpp_command(args):
    exe_path = os.path.join(os.getcwd(), "social.exe")
    if not os.path.exists(exe_path):
        return "⚠️ Error: social.exe not found. Please compile your C++ file first."
    try:
        result = subprocess.run(
            [exe_path] + args,
            capture_output=True,
            text=True,
            timeout=5
        )
        if result.stdout is None:
            return f"⚠️ No output received from backend (args={args})."
        output = result.stdout.strip()
        if not output:
            if result.stderr:
                return f"⚠️ Backend error: {result.stderr.strip()}"
            return f"⚠️ No output from backend for command {args}."
        return output
    except subprocess.TimeoutExpired:
        return "⚠️ The backend took too long to respond."
    except Exception as e:
        return f"⚠️ Error running backend: {type(e).__name__}: {e}"

@app.route("/")
def index():
    return render_template("index.html")

# ---- Utility to safely extract JSON ----
def get_json_safe():
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return None
    return data

# ---- Routes ----

@app.route("/add_user", methods=["POST"])
def add_user():
    data = get_json_safe()
    if not data or "name" not in data:
        return jsonify({"result": "❌ Invalid request data!"})
    name = data.get("name", "").strip()
    if not name:
        return jsonify({"result": "❌ Please enter a valid name!"})
    output = run_cpp_command(["add_user", name])
    return jsonify({"result": output})

@app.route("/add_friend", methods=["POST"])
def add_friend():
    data = get_json_safe()
    if not data:
        return jsonify({"result": "❌ Invalid request data!"})
    user1 = data.get("user1", "").strip()
    user2 = data.get("user2", "").strip()
    if not user1 or not user2:
        return jsonify({"result": "❌ Please provide both usernames!"})
    output = run_cpp_command(["add_friend", user1, user2])
    return jsonify({"result": output})

@app.route("/show_friends", methods=["POST"])
def show_friends():
    data = get_json_safe()
    if not data or "name" not in data:
        return jsonify({"result": "❌ Invalid request data!"})
    name = data.get("name", "").strip()
    if not name:
        return jsonify({"result": "❌ Please enter a username!"})
    output = run_cpp_command(["show_friends", name])
    return jsonify({"result": output})

@app.route("/find_path", methods=["POST"])
def find_path():
    data = get_json_safe()
    if not data:
        return jsonify({"result": "❌ Invalid request data!"})
    start = data.get("start", "").strip()
    end = data.get("end", "").strip()
    if not start or not end:
        return jsonify({"result": "❌ Enter both user names!"})
    output = run_cpp_command(["shortest_path", start, end])
    return jsonify({"result": output})

@app.route("/recommend", methods=["POST"])
def recommend():
    data = get_json_safe()
    if not data or "name" not in data:
        return jsonify({"result": "❌ Invalid request data!"})
    name = data.get("name", "").strip()
    if not name:
        return jsonify({"result": "❌ Please enter a username!"})
    output = run_cpp_command(["recommend", name])
    return jsonify({"result": output})

@app.route("/remove_user", methods=["POST"])
def remove_user():
    data = get_json_safe()
    if not data or "name" not in data:
        return jsonify({"result": "❌ Invalid request data!"})
    name = data.get("name", "").strip()
    if not name:
        return jsonify({"result": "❌ Please enter a username!"})
    output = run_cpp_command(["remove_user", name])
    return jsonify({"result": output})

@app.route("/remove_friend", methods=["POST"])
def remove_friend():
    data = get_json_safe()
    if not data:
        return jsonify({"result": "❌ Invalid request data!"})
    user1 = data.get("user1", "").strip()
    user2 = data.get("user2", "").strip()
    if not user1 or not user2:
        return jsonify({"result": "❌ Enter both usernames!"})
    output = run_cpp_command(["remove_friend", user1, user2])
    return jsonify({"result": output})

if __name__ == "__main__":
    app.run(debug=True)
