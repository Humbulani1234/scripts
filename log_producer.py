#!/usr/bin/env python3
"""
A Kafka producer that tails the system log file on Ubuntu, parses each line
to extract details (timestamp, host, process, pid, and message), and sends the
JSON record to a Kafka topic for real-time processing.
"""

import re
import json
import time
from kafka import KafkaProducer

# Regular expression to parse the log line.
LOG_REGEX = re.compile(
    r'^(?P<timestamp>\S+)\s+'          # ISO8601 timestamp
    r'(?P<host>\S+)\s+'                # Hostname
    r'(?P<process>[^\[\]:]+)'          # Process name (non-greedy)
    r'(?:\[(?P<pid>\d+)\])?:\s+'        # Optional [pid]
    r'(?P<message>.+)$'                # Log message
)

# Configure Kafka producer
producer = KafkaProducer(
    bootstrap_servers=['localhost:9092'],
    value_serializer=lambda v: json.dumps(v).encode('utf-8')
)

def parse_log_line(line: str) -> dict:
    """Parse a single log line into its constituent fields."""
    match = LOG_REGEX.match(line)
    if not match:
        return {}
    data = match.groupdict()
    
    # Optionally, extract event-level information from the message if present.
    # For example, if the message contains '<info>' or '<warning>', etc.
    event_match = re.search(r'<(\w+)>', data['message'])
    data['event_type'] = event_match.group(1) if event_match else None
    return data

def tail_f(path: str):
    """Generator function that yields new lines in a file as they are written."""
    with open(path, 'r') as f:
        # Go to the end of file
        f.seek(0, 2)
        while True:
            line = f.readline()
            if not line:
                time.sleep(0.1)
                continue
            yield line.rstrip()

if __name__ == '__main__':
    LOG_FILE_PATH = '/var/log/sys.log'  # Adjust the path as needed
    TOPIC = 'system_logs'
    print(f"Starting log tailing on {LOG_FILE_PATH}, sending to Kafka topic '{TOPIC}'")
    try:
        for line in tail_f(LOG_FILE_PATH):
            record = parse_log_line(line)
            if record:
                # Send the parsed record as a JSON message
                producer.send(TOPIC, record)
                # Flush periodically if needed
                producer.flush()
                print(f"Sent record: {record}")
    except KeyboardInterrupt:
        print("Log tailing stopped by user.")
