/*
MIT License

Copyright (c) 2019 Meng Rao <raomeng1@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <limits>
#include <memory>

namespace admincmd {

template<uint32_t recv_buff_size>
class socket_tcp_connection_c
{
public:
  ~socket_tcp_connection_c() { close("destruct"); }

  const char* get_last_error() { return last_error_; };

  bool is_connected() { return fd_ >= 0; }

  bool get_peer_name(struct sockaddr_in& addr) {
    socklen_t addr_len = sizeof(addr);
    return ::getpeername(fd_, (struct sockaddr*)&addr, &addr_len) == 0;
  }

  void close(const char* reason, bool check_errno = false) {
    if (fd_ >= 0) {
      save_error(reason, check_errno);
      ::close(fd_);
      fd_ = -1;
    }
  }

  bool write(const char* data, uint32_t size, bool more = false) {
    int flags = MSG_NOSIGNAL;
    if (more) flags |= MSG_MORE;
    do {
      int sent = ::send(fd_, data, size, flags);
      if (sent < 0) {
        if (errno != EAGAIN) {
          close("send error", true);
          return false;
        }
        continue;
      }
      data += sent;
      size -= sent;
    } while (size != 0);
    return true;
  }

  template<typename Handler>
  bool read(Handler handler) {
    int ret = ::read(fd_, recvbuf_ + tail_, recv_buff_size - tail_);
    if (ret <= 0) {
      if (ret < 0 && errno == EAGAIN) return false;
      if (ret < 0) {
        close("read error", true);
      }
      else {
        close("remote close");
      }
      return false;
    }
    tail_ += ret;

    uint32_t remaining = handler(recvbuf_ + head_, tail_ - head_);
    if (remaining == 0) {
      head_ = tail_ = 0;
    }
    else {
      head_ = tail_ - remaining;
      if (head_ >= recv_buff_size / 2) {
        memcpy(recvbuf_, recvbuf_ + head_, remaining);
        head_ = 0;
        tail_ = remaining;
      }
      else if (tail_ == recv_buff_size) {
        close("recv buf full");
      }
    }
    return true;
  }

protected:
  template<uint32_t>
  friend class socket_tcp__serverc;

  bool open(int fd) {
    fd_ = fd;
    head_ = tail_ = 0;

    int flags = fcntl(fd_, F_GETFL, 0);
    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) < 0) {
      close("fcntl O_NONBLOCK error", true);
      return false;
    }

    int yes = 1;
    if (setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) < 0) {
      close("setsockopt TCP_NODELAY error", true);
      return false;
    }

    return true;
  }

  void save_error(const char* msg, bool check_errno) {
    snprintf(last_error_, sizeof(last_error_), "%s %s", msg, check_errno ? (const char*)strerror(errno) : "");
  }

  int fd_ = -1;
  uint32_t head_;
  uint32_t tail_;
  char recvbuf_[recv_buff_size];
  char last_error_[64] = "";
};

template<uint32_t recv_buff_size = 4096>
class socket_tcp__serverc
{
public:
  using tcp_connection = socket_tcp_connection_c<recv_buff_size>;

  bool init(const char* interface, const char* _serverip, uint16_t _serverport) {
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd_ < 0) {
      save_error("socket error");
      return false;
    }

    int flags = fcntl(listenfd_, F_GETFL, 0);
    if (fcntl(listenfd_, F_SETFL, flags | O_NONBLOCK) < 0) {
      close("fcntl O_NONBLOCK error");
      return false;
    }

    int yes = 1;
    if (setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
      close("setsockopt SO_REUSEADDR error");
      return false;
    }

    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    inet_pton(AF_INET, _serverip, &(local_addr.sin_addr));
    local_addr.sin_port = htons(_serverport);
    bzero(&(local_addr.sin_zero), 8);
    if (bind(listenfd_, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
      close("bind error");
      return false;
    }
    if (listen(listenfd_, 5) < 0) {
      close("listen error");
      return false;
    }

    return true;
  };

  void close(const char* reason) {
    if (listenfd_ >= 0) {
      save_error(reason);
      ::close(listenfd_);
      listenfd_ = -1;
    }
  }

  const char* get_last_error() { return last_error_; };

  ~socket_tcp__serverc() { close("destruct"); }

  bool accept2(tcp_connection& conn) {
    struct sockaddr_in clientaddr;
    socklen_t addr_len = sizeof(clientaddr);
    int fd = ::accept(listenfd_, (struct sockaddr*)&(clientaddr), &addr_len);
    if (fd < 0) {
      return false;
    }
    if (!conn.open(fd)) {
      return false;
    }
    return true;
  }

private:
  void save_error(const char* msg) { snprintf(last_error_, sizeof(last_error_), "%s %s", msg, strerror(errno)); }

  int listenfd_ = -1;
  char last_error_[64] = "";
};

template<typename event_handler, typename conn_user_data = char, uint32_t max_cmd_len = 4096, uint32_t max_conns = 10>
class admin_cmd_server_c
{
public:
  using tcp_server = socket_tcp__serverc<max_cmd_len>;

  class connection_c
  {
  public:
    conn_user_data user_data;

    // get remote network address
    // return true on success
    bool get_peer_name(struct sockaddr_in& addr) { return conn.get_peer_name(addr); }

    // more = true: MSG_MORE flag is set on this write
    // return true on success
    bool write(const char* data, uint32_t size, bool more = false) { return conn.write(data, size, more); }

    // close this connection with a reason
    void close(const char* reason = "user close") { conn.close(reason); }

  private:
    friend class admin_cmd_server_c;

    int64_t expire;
    typename tcp_server::tcp_connection conn;
  };

  admin_cmd_server_c() {
    for (uint32_t i = 0; i < max_conns; i++) {
      conns_[i] = conns_data_ + i;
    }
  }

  // conn_timeout: connection max inactive time in seconds, 0 means no limit
  bool init(const char* _serverip, uint16_t _serverport, int64_t conn_timeout = 0) {
    _conn_timeout = conn_timeout;
    return _server.init("", _serverip, _serverport);
  }

  const char* get_last_error() { return _server.get_last_error(); }

  void poll(event_handler* handler) {
    int64_t now = 0;
    int64_t expire = std::numeric_limits<int64_t>::max();
    if (_conn_timeout) {
      now = time(0);
      expire = now + _conn_timeout;
    }
    if (conns_cnt_ < max_conns) {
      connection_c& new_conn = *conns_[conns_cnt_];
      if (_server.accept2(new_conn.conn)) {
        new_conn.expire = expire;
        handler->on_admin_connect(new_conn);
        conns_cnt_++;
      }
    }
    for (uint32_t i = 0; i < conns_cnt_;) {
      connection_c& conn = *conns_[i];
      conn.conn.read([&](const char* data, uint32_t size) {
        const char* data_end = data + size;
        char buf[max_cmd_len] = {0};
        const char* argv[max_cmd_len];
        char* out = buf + 1;
        int argc = 0;
        bool in_quote = false;
        bool single_quote = false;
        while (data < data_end) {
          char ch = *data++;
          if (!in_quote) {
            if (ch == ' ' || ch == '\r')
              *out++ = 0;
            else if (ch == '\n') {
              if (argc) {
                *out = 0;
                handler->on_admin_cmd(conn, argc, argv);
              }
              conn.expire = expire;
              out = buf + 1;
              argc = 0;
              in_quote = false;
              size = data_end - data;
            }
            else {
              if (*(out - 1) == 0) argv[argc++] = out;
              if (ch == '\'')
                in_quote = single_quote = true;
              else if (ch == '"')
                in_quote = true;
              else if (ch == '\\')
                *out++ = *data++;
              else
                *out++ = ch;
            }
          }
          else {
            if (single_quote) {
              if (ch == '\'')
                in_quote = single_quote = false;
              else
                *out++ = ch;
            }
            else {
              if (ch == '"')
                in_quote = false;
              else if (ch == '\\' && (*data == '\\' || *data == '"'))
                *out++ = *data++;
              else
                *out++ = ch;
            }
          }
        }
        return size;
      });
      if (now > conn.expire) conn.conn.close("timeout");
      if (conn.conn.is_connected())
        i++;
      else {
        handler->on_admin_disconnect(conn, conn.conn.get_last_error());
        std::swap(conns_[i], conns_[--conns_cnt_]);
      }
    }
  }

private:
  int64_t _conn_timeout;
  tcp_server _server;

  uint32_t conns_cnt_ = 0;
  connection_c* conns_[max_conns];
  connection_c conns_data_[max_conns];
};

} // namespace admincmd