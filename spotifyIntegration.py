from dotenv import load_dotenv
import os
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import serial.tools.list_ports
import serial
import time
import re
import threading

# Load .env file
load_dotenv()

def find_arduino_port():
    """Find Arduino port automatically"""
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        # Check for common Arduino identifiers
        if any(identifier in p.device.lower() for identifier in ['usbmodem', 'usbserial', 'arduino']):
            return p.device
        # Also check description
        if p.description and any(identifier in p.description.lower() for identifier in ['arduino', 'ch340', 'cp210']):
            return p.device
    return None

def clean_song_name(song_name):
    """Clean song name for LED display - remove special characters and limit length"""
    # Remove common patterns like (feat. Artist), [Remix], etc.
    song_name = re.sub(r'\(.*?\)', '', song_name)
    song_name = re.sub(r'\[.*?\]', '', song_name)
    
    # Remove special characters except spaces and basic punctuation
    song_name = re.sub(r'[^\w\s\-\.\']', '', song_name)
    
    # Clean up extra spaces
    song_name = ' '.join(song_name.split())
    
    # Limit length for reasonable scrolling
    if len(song_name) > 50:
        song_name = song_name[:50] + "..."
    
    return song_name.strip().upper()

def handle_spotify_command(command):
    """Handle button commands from Arduino"""
    try:
        if command == "PLAY_PAUSE":
            # Get current playback state
            playback = sp.current_playback()
            if playback and playback.get('is_playing'):
                sp.pause_playback()
                print("⏸️  Music paused")
            else:
                sp.start_playback()
                print("▶️  Music playing")
                
        elif command == "SKIP_FORWARD":
            sp.next_track()
            print("⏭️  Skipped forward")
            
        elif command == "SKIP_BACK":
            sp.previous_track()
            print("⏮️  Skipped backward")
            
        elif command == "VOLUME_UP":
            # Get current volume
            devices = sp.devices()
            if devices['devices']:
                current_volume = devices['devices'][0].get('volume_percent', 50)
                new_volume = min(100, current_volume + 5)  # Increase by 5%
                sp.volume(new_volume)
                print(f"🔊 Volume: {new_volume}%")
            
        elif command == "VOLUME_DOWN":
            # Get current volume
            devices = sp.devices()
            if devices['devices']:
                current_volume = devices['devices'][0].get('volume_percent', 50)
                new_volume = max(0, current_volume - 5)  # Decrease by 5%
                sp.volume(new_volume)
                print(f"🔉 Volume: {new_volume}%")
            
    except Exception as e:
        print(f"Error handling command {command}: {e}")

def listen_for_commands():
    """Listen for commands from Arduino in a separate thread"""
    while True:
        try:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line.startswith("CMD:"):
                    command = line[4:]  # Remove "CMD:" prefix
                    print(f"🎮 Button command: {command}")
                    handle_spotify_command(command)
                elif line and not line.startswith("Arduino"):
                    print(f"Arduino: {line}")
        except Exception as e:
            print(f"Error reading from Arduino: {e}")
        time.sleep(0.1)

# Initialize Spotify
print("Initializing Spotify connection...")
try:
    sp = spotipy.Spotify(auth_manager=SpotifyOAuth(
        client_id=os.getenv("SPOTIPY_CLIENT_ID"),
        client_secret=os.getenv("SPOTIPY_CLIENT_SECRET"),
        redirect_uri=os.getenv("SPOTIPY_REDIRECT_URI"),
        scope="user-read-playback-state user-read-currently-playing user-modify-playback-state"
    ))
    
    # Test connection
    current_user = sp.current_user()
    print(f"✓ Connected to Spotify as: {current_user['display_name']}")
    
except Exception as e:
    print(f"✗ Error connecting to Spotify: {e}")
    print("Please check your .env file has correct Spotify credentials")
    exit(1)

# Find and connect to Arduino
print("\nSearching for Arduino...")
arduino_port = find_arduino_port()

if not arduino_port:
    print("✗ No Arduino found. Please check:")
    print("  - Arduino is connected via USB")
    print("  - Arduino drivers are installed")
    print("  - Arduino is not being used by another program")
    
    # Show available ports for debugging
    ports = list(serial.tools.list_ports.comports())
    if ports:
        print("\nAvailable ports:")
        for p in ports:
            print(f"  {p.device} - {p.description}")
    exit(1)

print(f"✓ Found Arduino on {arduino_port}")

# Connect to Arduino
try:
    ser = serial.Serial(
        port=arduino_port,
        baudrate=115200,
        timeout=2,
        write_timeout=2
    )
    
    # Wait for Arduino to initialize
    print("Waiting for Arduino to initialize...")
    time.sleep(3)
    
    # Clear any startup messages
    while ser.in_waiting:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(f"Arduino: {line}")
    
    print("✓ Arduino connection established")
    
except Exception as e:
    print(f"✗ Error connecting to Arduino: {e}")
    exit(1)

def send_text_to_arduino(text):
    """Send text to Arduino for display"""
    try:
        # Send text followed by newline
        message = f"{text}\n"
        ser.write(message.encode('utf-8'))
        ser.flush()
        
        # Brief pause to allow Arduino to process
        time.sleep(0.1)
        
        # Read any response from Arduino
        while ser.in_waiting:
            response = ser.readline().decode('utf-8', errors='ignore').strip()
            if response:
                print(f"Arduino: {response}")
                
    except Exception as e:
        print(f"Error sending to Arduino: {e}")

# Main loop
print("\n" + "="*50)
print("Spotify LED Display with Button Controls")
print("Button Controls:")
print("  - Single press: Play/Pause")
print("  - Double press L/R: Skip back/forward") 
print("  - Hold L/R: Volume down/up")
print("Press Ctrl+C to stop")
print("="*50)

current_song = None
last_update = 0

# Start command listening thread
command_thread = threading.Thread(target=listen_for_commands, daemon=True)
command_thread.start()

try:
    while True:
        try:
            # Get current playback (limit API calls)
            if time.time() - last_update > 2:  # Update every 2 seconds
                playback = sp.current_playback()
                last_update = time.time()
                
                if playback and playback.get('is_playing') and playback.get('item'):
                    # Extract song info
                    track = playback['item']
                    song_name = track['name']
                    artist_name = track['artists'][0]['name'] if track['artists'] else "Unknown Artist"
                    
                    # Create display text
                    display_text = f"{song_name} - {artist_name}"
                    display_text = clean_song_name(display_text)
                    
                    # Only update if song changed
                    if display_text != current_song:
                        current_song = display_text
                        print(f"\n♪ Now Playing: {current_song}")
                        send_text_to_arduino(current_song)
                        
                else:
                    # No music playing
                    if current_song != "NO SONG PLAYING":
                        current_song = "NO SONG PLAYING"
                        print(f"\n♪ {current_song}")
                        send_text_to_arduino(current_song)
            
            time.sleep(0.5)  # Small delay to prevent excessive CPU usage
            
        except spotipy.exceptions.SpotifyException as e:
            print(f"Spotify API error: {e}")
            if "401" in str(e):
                print("Token expired, refreshing...")
                sp.auth_manager.refresh_access_token(sp.auth_manager.get_access_token()['refresh_token'])
            time.sleep(5)
            
        except Exception as e:
            print(f"Unexpected error: {e}")
            time.sleep(2)
            
except KeyboardInterrupt:
    print("\n\nStopping Spotify LED Display...")
    
    # Send goodbye message
    try:
        send_text_to_arduino("GOODBYE")
        time.sleep(1)
    except:
        pass
    
    # Close serial connection
    if ser.is_open:
        ser.close()
        print("✓ Arduino connection closed")
    
    print("✓ Stopped successfully")
    
except Exception as e:
    print(f"\nFatal error: {e}")
    if 'ser' in locals() and ser.is_open:
        ser.close()