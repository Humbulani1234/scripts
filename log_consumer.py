#!/usr/bin/env python3
"""
A Kafka consumer that listens for system log records on a given topic and,
in real time, inserts the parsed log information into a PostgreSQL database.
It also ensures that the required database, user, and table exist.
"""

import json
import time
import psycopg2
from kafka import KafkaConsumer

# PostgreSQL configuration – adjust credentials and connection parameters as needed.
DB_ADMIN_CONFIG = {
    'dbname': 'postgres',
    'user': 'postgres',
    'password': 'admin_password',  # Use secure password management in production!
    'host': 'localhost'
}
# New role and database for logs processing
LOG_DB_CONFIG = {
    'dbname': 'logsdb',
    'user': 'log_user',
    'password': 'log_password',
    'host': 'localhost'
}

TABLE_NAME = 'system_logs'

def create_db_and_user():
    """Create the logs database and user if they do not already exist."""
    with psycopg2.connect(**DB_ADMIN_CONFIG) as conn:
        conn.autocommit = True
        with conn.cursor() as cur:
            # Create the user if not exists
            cur.execute(
                "DO $$ BEGIN "
                "IF NOT EXISTS (SELECT FROM pg_catalog.pg_roles WHERE rolname = %s) THEN "
                "CREATE ROLE %s LOGIN PASSWORD %s; "
                "END IF; "
                "END $$;",
                (LOG_DB_CONFIG['user'], LOG_DB_CONFIG['user'], LOG_DB_CONFIG['password'])
            )
            # Create the database if not exists
            cur.execute(
                "SELECT 1 FROM pg_database WHERE datname = %s;", (LOG_DB_CONFIG['dbname'],)
            )
            if not cur.fetchone():
                cur.execute(f"CREATE DATABASE {LOG_DB_CONFIG['dbname']};")
    print(f"Ensured that database '{LOG_DB_CONFIG['dbname']}' and user '{LOG_DB_CONFIG['user']}' exist.")

def create_table(conn):
    """Create the system_logs table if it does not exist."""
    with conn.cursor() as cur:
        cur.execute(f"""
            CREATE TABLE IF NOT EXISTS {TABLE_NAME} (
                id SERIAL PRIMARY KEY,
                timestamp TIMESTAMPTZ NOT NULL,
                host TEXT,
                process TEXT,
                pid INTEGER,
                event_type TEXT,
                message TEXT
            );
        """)
        conn.commit()
    print(f"Ensured that table '{TABLE_NAME}' exists in database '{LOG_DB_CONFIG['dbname']}'.")

def insert_log_record(conn, record: dict):
    """Insert a log record into the PostgreSQL table."""
    with conn.cursor() as cur:
        cur.execute(f"""
            INSERT INTO {TABLE_NAME} (timestamp, host, process, pid, event_type, message)
            VALUES (%s, %s, %s, %s, %s, %s);
        """, (
            record.get('timestamp'),
            record.get('host'),
            record.get('process'),
            int(record['pid']) if record.get('pid') and record['pid'].isdigit() else None,
            record.get('event_type'),
            record.get('message')
        ))
        conn.commit()

def main():
    # Ensure that the database and user are set up.
    create_db_and_user()

    # Connect to the logs database.
    conn = psycopg2.connect(**LOG_DB_CONFIG)
    create_table(conn)

    # Initialize Kafka consumer.
    consumer = KafkaConsumer(
        'system_logs',
        bootstrap_servers=['localhost:9092'],
        value_deserializer=lambda m: json.loads(m.decode('utf-8')),
        auto_offset_reset='earliest',
        enable_auto_commit=True,
        group_id='log_consumers'
    )
    print("Kafka consumer started. Waiting for log messages...")

    try:
        for message in consumer:
            record = message.value
            # Optionally, further validation or processing can be added here.
            insert_log_record(conn, record)
            print(f"Inserted record into DB: {record}")
    except KeyboardInterrupt:
        print("Consumer stopped by user.")
    finally:
        conn.close()

if __name__ == '__main__':
    main()
