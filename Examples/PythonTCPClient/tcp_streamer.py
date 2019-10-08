import socket


class TCPStreamer:
    def listen(self, server_address):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind(server_address)
        self.socket.listen(1)

        self.connection = None

    def recvMessage(self):

        def recvall(conn, remains):
            buf = bytearray()
            while remains:
                data = conn.recv(remains)
                if not data:
                    conn.close()
                    return None
                buf += data
                remains -= len(data)
            return buf

        # if we dont have a connection wait for it
        if not self.connection or self.connection.fileno() == -1:
            self.connection, self.client_address = self.socket.accept()

        # wait for one message
        data = recvall(self.connection, 4)
        if not data:
            return None
        length = int.from_bytes(data, byteorder='big')
        message = recvall(self.connection, length)

        if not message:
            return None

        return message
