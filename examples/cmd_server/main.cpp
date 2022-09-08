#include <iostream>
#include <crate/app/version_v1.hpp>
#include <crate/metrics/reading_v1.hpp>
#include <chrono>

#include <crate/externals/admincmd/admin_cmd_server.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <signal.h>

class server_c
{
public:
  struct cmd_conn_data_s
  {
    bool login;
  };
  using admin_server_c = admincmd::admin_cmd_server_c<server_c, cmd_conn_data_s>;
  using admin_conn_c = admin_server_c::connection_c;

  void run() {
    if (!admincmd_server.init("0.0.0.0", 1234)) {
      std::cout << "admincmd_server init failed: " << admincmd_server.get_last_error() << std::endl;
      return;
    }
    admincmd_help = "Server help:\n"
                    "login password\n"
                    "echo str\n"
                    "stop\n";

    running = true;
    admincmd_thr = std::thread([this]() {
      while (running.load(std::memory_order_relaxed)) {
        admincmd_server.poll(this);
        std::this_thread::yield();
      }
    });

    std::cout << "Server running..." << std::endl;
    admincmd_thr.join();
    std::cout << "Server stopped..." << std::endl;
  }

  void stop() { running = false; }

  void on_admin_connect(admin_conn_c& conn) {
    struct sockaddr_in addr;
    conn.get_peer_name(addr);
    std::cout << "admin connection from: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;

    conn.user_data.login = false;
    conn.write(admincmd_help.data(), admincmd_help.size());
  }

  void on_admin_disconnect(admin_conn_c& conn, const std::string& error) {
    struct sockaddr_in addr;
    conn.get_peer_name(addr);
    std::cout << "admin disconnect, error: " << error << ", login state: " << conn.user_data.login << std::endl;
  }

  void on_admin_cmd(admin_conn_c& conn, int argc, const char** argv) {
    std::string resp = on_cmd(conn.user_data, argc, argv);
    if (resp.size()) {
      if (resp.back() != '\n') resp.push_back('\n');
      conn.write(resp.data(), resp.size());
    }
  }

private:
  std::string on_cmd(cmd_conn_data_s& conn, int argc, const char** argv) {
    if (!strcmp(argv[0], "help")) {
      return admincmd_help;
    }
    else if (!strcmp(argv[0], "login")) {
      if (argc < 2 || strcmp(argv[1], "123456")) {
        return "wrong password";
      }
      else {
        conn.login = true;
        return "login success";
      }
    }
    else if (!conn.login) {
      return "must login first";
    }
    else if (!strcmp(argv[0], "echo") && argc >= 2) {
      return argv[1];
    }
    else if (!strcmp(argv[0], "stop")) {
      stop();
      return "bye";
    }
    else {
      return "invalid cmd, check help";
    }
  }

private:
  admin_server_c admincmd_server;
  std::thread admincmd_thr;
  std::string admincmd_help;
  std::atomic<bool> running;
};

server_c server;

void my_handler(int s) {
  server.stop();
}

int main(int argc, char** argv) {
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  server.run();
}