#ifndef MAIN
#define MAIN

#include <mpi.h>
#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <bits/stdc++.h> 
#include <algorithm>
#include <string>
#include <stddef.h>
#include <stdio.h>


#endif
//TAGS
#define ACK 1
#define SERVER 2
#define START_LEADER_ELECTION 3
#define LEADER_ELECTION_DONE 4
#define CLIENT 5
#define PROBE 6
#define REPLY 7
#define SET_LEADER 8
#define CONNECT 9
#define GET_CHILD 10
#define PRINT 11
#define ORDER 12
#define SUPPLY 13
#define SUPPLY_ACK 14
#define EXTERNAL_SUPPLY 15
#define SUPPLY_REQUEST 16
#define REPORT 17
#define TERMINATE 18

using namespace std;

typedef struct info_s {
  int left_neighbour;
  int right_neighbour;
  int leaders_rank;
  int id;
  int curr_phase_neighbourhoud;
  int hop_counter;
  int message_from;
  int supplies_order;
  int got_supplies;
  int initial_message_from;
} info;

void reset(info init){
  init.left_neighbour = -1;
  init.right_neighbour = -1;
  init.leaders_rank = -1;
  init.id = -1;
  init.curr_phase_neighbourhoud = -1;
  init.hop_counter = -1;
  init.message_from = -1;
  init.got_supplies = -1;
  init.supplies_order = -1;
  init.initial_message_from = -1;
}

int main(int argc, char *argv[]) {
  
  int rank;
  int num_of_ranks;
  int params[2];
  char ack_buffer[4] = "ACK";

  MPI_Init(&argc, &argv);
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_of_ranks);

  /* create a type for struct info */
  const int nitems = 10;
  int blocklengths[10] ={1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  MPI_Datatype types[10] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
  MPI_Datatype MPI_INFO_TYPE;
  MPI_Aint     offsets[10];

  offsets[0] = offsetof(info, left_neighbour);
  offsets[1] = offsetof(info, right_neighbour);
  offsets[2] = offsetof(info, leaders_rank);
  offsets[3] = offsetof(info, id);
  offsets[4] = offsetof(info, curr_phase_neighbourhoud);
  offsets[5] = offsetof(info, hop_counter);
  offsets[6] = offsetof(info, message_from);
  offsets[7] = offsetof(info, supplies_order);
  offsets[8] = offsetof(info, got_supplies);
  offsets[9] = offsetof(info, initial_message_from);

  MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_INFO_TYPE);
  MPI_Type_commit(&MPI_INFO_TYPE);
 
  info send_info, receive_info;
  if (rank == 0) 
  {
    int count = 0;
    int send_to = -1;
    int leaders_rank = -1;
    string line_of_file, read_line, new_token;
    vector <string> tokens; 
    ifstream MyReadFile("test1.txt");
    vector<int> servers_ranks;
    servers_ranks.reserve(atoi(argv[1]));

    if(MyReadFile.is_open())
    {
      while (getline (MyReadFile, read_line))
      {
        reset(send_info);
        tokens.clear();
        istringstream line_of_file(read_line);
      
        while(getline(line_of_file, new_token, ' '))
          tokens.push_back(new_token); 
      
        if(!tokens.at(0).compare("SERVER"))
        {   
            ++count;
            cout << tokens.at(0) << " " << tokens.at(1) << " " << tokens.at(2) << " " << tokens.at(3) << endl;
            send_to  = atoi(tokens.at(1).c_str());
            servers_ranks.push_back(send_to);
            send_info.left_neighbour = atoi(tokens.at(2).c_str());
            send_info.right_neighbour = atoi(tokens.at(3).c_str());

            MPI_Send(&send_info, 1, MPI_INFO_TYPE, send_to, SERVER, MPI_COMM_WORLD);
            MPI_Recv(ack_buffer, 4, MPI_CHAR, send_to, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            cout << "The Server "<< send_to << " is up!" << endl; 
            --count;     
        } 
        else if (!tokens.at(0).compare("START_LEADER_ELECTION"))
        {
          ++count;
          cout  << tokens.at(0) <<endl;
          for(int i = 0; i < atoi(argv[1]); ++i)
            MPI_Send(&send_info, 1, MPI_INFO_TYPE, servers_ranks.at(i), START_LEADER_ELECTION, MPI_COMM_WORLD);
          MPI_Recv(&leaders_rank, 1, MPI_INT, MPI_ANY_SOURCE, LEADER_ELECTION_DONE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          cout  << "LEADER_ELECTION_DONE" << endl <<"LEADER IS: " << leaders_rank <<endl;
          cout << "------------------------------" << endl;
          for(int i = 1; i < num_of_ranks; ++i)
          {
            if(!(find(servers_ranks.begin(), servers_ranks.end(), i) != servers_ranks.end())) 
            {
                send_info.leaders_rank = leaders_rank;
                MPI_Send(&send_info, 1, MPI_INFO_TYPE, i, CLIENT, MPI_COMM_WORLD);
                MPI_Recv(ack_buffer, 4, MPI_CHAR, MPI_ANY_SOURCE, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cout << "The "<< i << " is CLIENT!" << endl; 
            }
          }
          --count; 
        }
        else if (!tokens.at(0).compare("CONNECT"))
        {
          ++count;
          cout << tokens.at(0) << " " << tokens.at(1) << " " << tokens.at(2) << endl;
          send_to  = atoi(tokens.at(1).c_str());
          send_info.id = atoi(tokens.at(2).c_str());

          MPI_Send(&send_info, 1, MPI_INFO_TYPE, send_to, CONNECT, MPI_COMM_WORLD);
          MPI_Recv(ack_buffer, 4, MPI_CHAR, send_to, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          
          cout << "The CLIENT "<< send_to << " connected to: "<< send_info.id << endl; 
          --count;        
        }
        else if (!tokens.at(0).compare("ORDER"))
        {
          ++count;
          cout << tokens.at(0) << "  " << tokens.at(1) << " " << tokens.at(2) << endl;
          send_to  = atoi(tokens.at(1).c_str());
          send_info.supplies_order = atoi(tokens.at(2).c_str());
          send_info.initial_message_from = send_to;
          send_info.got_supplies = 0;
          send_info.id = 0;
          MPI_Send(&send_info, 1, MPI_INFO_TYPE, send_to, ORDER, MPI_COMM_WORLD);
          MPI_Recv(&send_info, 1, MPI_INFO_TYPE, send_to, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          if(send_info.id == 1)
          {
            cout << "SERVER "<< send_info.message_from << " DOESNT HAVE "<< atoi(tokens.at(2).c_str()) << endl;
            cout << "SERVER "<< send_info.message_from << " GAVE "<< send_info.supplies_order << endl;
            cout << "ORDER  "<< send_info.initial_message_from << " HAS LEFT "<< send_info.got_supplies << endl;
          }
          else
            cout << "SERVER "<< send_info.message_from << " SOLD "<< send_info.supplies_order << endl;
          --count;   
        }
        else if (!tokens.at(0).compare("PRINT"))
        {
          ++count;
          cout << tokens.at(0) << endl;
          MPI_Send(&send_info, 1, MPI_INFO_TYPE, leaders_rank, PRINT, MPI_COMM_WORLD);
          for(int i = 0; i < atoi(argv[1]); ++i)
          {
            MPI_Recv(&send_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "SERVER "<< send_info.message_from << " HAS QUANTITY " << send_info.got_supplies << endl; 
          }
          --count; 
        }
        else if (!tokens.at(0).compare("SUPPLY"))
        {
          ++count;
          cout << tokens.at(0) << " " << tokens.at(1) << " " << tokens.at(2) << endl;
          send_to  = atoi(tokens.at(1).c_str());
          send_info.supplies_order = atoi(tokens.at(2).c_str());
          send_info.message_from = send_to;
          send_info.initial_message_from = send_to;
          MPI_Send(&send_info, 1, MPI_INFO_TYPE, send_to, SUPPLY, MPI_COMM_WORLD);
          MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, send_to, SUPPLY_ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          cout << "SERVER "<< receive_info.initial_message_from << " RECIEVED "<< receive_info.supplies_order << endl;  
          --count; 
        }
        else if (!tokens.at(0).compare("EXTERNAL_SUPPLY"))
        {
          while(count != 0){}
          ++count;
          cout << tokens.at(0) << " " << tokens.at(1) << endl;
          send_info.got_supplies = atoi(tokens.at(1).c_str());
          send_info.message_from = leaders_rank;
          send_info.initial_message_from = leaders_rank;
          send_info.id = 0;
          MPI_Send(&send_info, 1, MPI_INFO_TYPE, leaders_rank, EXTERNAL_SUPPLY, MPI_COMM_WORLD);
          int helper = 0;
          for(int i = 0; i < atoi(argv[1]) + 1; ++i)
          {
            MPI_Recv(&send_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(send_info.id == 0)
            {
              cout << "SERVER "<< send_info.message_from << " GOT EXTERNAL SUPPLY " << send_info.supplies_order << endl; 
              if(send_info.supplies_order != 0)
                helper++;
            }
            else
            {
              helper++;
              cout << "SERVER "<< send_info.message_from << " GOT ALL EXTERNAL SUPPLY LEFT " << send_info.supplies_order << endl; 
            }
            if(send_info.id == 1)
            {
              cout << "EXTERNAL SUPPLY IS EMPTY. "<< atoi(argv[1]) - helper << " SERVERS DIDN'T RECIEVE ANY SUPPLY. EXTERNAL SUPPLY: " << send_info.got_supplies << endl; 
              break; 
            }
          }
          --count;
        }
        else if (!tokens.at(0).compare("REPORT"))
          {
            ++count;
            cout << tokens.at(0) <<endl;
            send_info.message_from = leaders_rank;
            send_info.initial_message_from = leaders_rank;
            MPI_Send(&send_info, 1, MPI_INFO_TYPE, leaders_rank, REPORT, MPI_COMM_WORLD);
            MPI_Recv(&send_info, 1, MPI_INFO_TYPE, leaders_rank, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "FINAL REPORT ALL CLIENTS BOUGHT " << send_info.supplies_order << endl; 
            --count;
          }
        cout << "------------------------------" << endl;
      }
      
      MyReadFile.close();

      for(int i = 1; i < num_of_ranks; ++i)
      {
        MPI_Send(&send_info, 1, MPI_INFO_TYPE, i, TERMINATE, MPI_COMM_WORLD);
        MPI_Recv(ack_buffer, 4, MPI_CHAR, MPI_ANY_SOURCE, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }

    }
    else
    {
      cout << "Unable to open file" << endl;; 
      assert(false);
    }
  }
  else
  {
    int i_am_server = false;

    //Server
    int my_old_supplies = -1;
    int supplies_to_get = -1;
    int server_supplies = -1;
    int leader_server = -1;
    int left_neighbour = -1, right_neighbour = -1;
    bool probe_sended = false;
    bool msg_send = false;

    //client
    int supplies_bought = 0;
    int my_parent = -1;
    vector<int> my_clients;
    int my_children = -1;
    bool first_reply = false;
    int get_all_supplies = 0;
    while(true)
    {
      MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      
      if      (status.MPI_TAG == SERVER)
      {
        i_am_server = true;
        server_supplies = 300;
        left_neighbour = receive_info.left_neighbour;
        right_neighbour = receive_info.right_neighbour;
        MPI_Send(ack_buffer, 4, MPI_CHAR, 0, ACK, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == START_LEADER_ELECTION)
      {
        if(!probe_sended)
        {
          send_info.message_from = rank;
          send_info.id = rank;
          send_info.hop_counter = 1;
          send_info.curr_phase_neighbourhoud = 0;
          MPI_Send(&send_info, 1, MPI_INFO_TYPE, left_neighbour, PROBE, MPI_COMM_WORLD);
          MPI_Send(&send_info, 1, MPI_INFO_TYPE, right_neighbour, PROBE, MPI_COMM_WORLD);
          probe_sended = true;
        }
      }
      else if (status.MPI_TAG == PROBE)
      {
        if(receive_info.id == rank){
          leader_server = rank;
          MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, SET_LEADER, MPI_COMM_WORLD);
          MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, right_neighbour, SET_LEADER, MPI_COMM_WORLD, &status);
          assert(receive_info.id == rank);
          MPI_Send(&rank, 1, MPI_INT, 0, LEADER_ELECTION_DONE, MPI_COMM_WORLD);
        } 
      
        if(receive_info.id > rank && receive_info.hop_counter < pow (2, receive_info.curr_phase_neighbourhoud))
        {
          receive_info.hop_counter++; 
          if(left_neighbour == receive_info.message_from)
          {
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, right_neighbour, PROBE, MPI_COMM_WORLD);
          }
          else if(right_neighbour == receive_info.message_from)
          {
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, PROBE, MPI_COMM_WORLD);
          }
          probe_sended = true;
        }
        else if(receive_info.id > rank && receive_info.hop_counter >= pow (2, receive_info.curr_phase_neighbourhoud))
        {
          if(left_neighbour == receive_info.message_from)
          {
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, REPLY, MPI_COMM_WORLD);
          }
          else if(right_neighbour == receive_info.message_from)
          {
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, right_neighbour, REPLY, MPI_COMM_WORLD);
          }
        }
      }
      else if (status.MPI_TAG == REPLY)
      {
        if(receive_info.id != rank) 
        {
          if(left_neighbour == receive_info.message_from)
          {
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, right_neighbour, REPLY, MPI_COMM_WORLD);
          }
          else if(right_neighbour == receive_info.message_from)
          {
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, REPLY, MPI_COMM_WORLD);
          }
        }
        else
        {
          if(first_reply)
          {
            receive_info.hop_counter = 1;
            receive_info.curr_phase_neighbourhoud++;
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, PROBE, MPI_COMM_WORLD);
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, right_neighbour, PROBE, MPI_COMM_WORLD);
            first_reply = false;
          }
          else
            first_reply = true;
        }
      }
      else if (status.MPI_TAG == SET_LEADER)
      {
        leader_server = receive_info.id;
        MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, SET_LEADER, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == CLIENT)
      {
        i_am_server = false;
        leader_server = receive_info.leaders_rank;
        MPI_Send(ack_buffer, 4, MPI_CHAR, 0, ACK, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == CONNECT)
      {
        my_parent = receive_info.id;
        receive_info.id = rank;
        MPI_Send(&receive_info, 1, MPI_INFO_TYPE, my_parent, GET_CHILD, MPI_COMM_WORLD);
        MPI_Recv(ack_buffer, 4, MPI_CHAR, my_parent, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(ack_buffer, 4, MPI_CHAR, 0, ACK, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == GET_CHILD)
      {
        my_clients.push_back(receive_info.id);
        MPI_Send(ack_buffer, 4, MPI_CHAR, receive_info.id, ACK, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == PRINT)
      {
        MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, PRINT, MPI_COMM_WORLD);
        
        if(leader_server == rank)
          MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, right_neighbour, PRINT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //assignments must be here
        receive_info.got_supplies = server_supplies;
        receive_info.message_from = rank;
        MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == ORDER)
      {
        if(my_parent != -1)
          MPI_Send(&receive_info, 1, MPI_INFO_TYPE, my_parent, ORDER, MPI_COMM_WORLD);
        else
        {
          receive_info.message_from = rank;

          if(server_supplies >= receive_info.supplies_order)
            server_supplies -= receive_info.supplies_order;
          else
          {
            receive_info.got_supplies = receive_info.supplies_order - server_supplies;
            receive_info.supplies_order = server_supplies;
            server_supplies = 0;
            receive_info.id = 1;
          }

          MPI_Send(&receive_info, 1, MPI_INFO_TYPE, receive_info.initial_message_from, ACK, MPI_COMM_WORLD);
        }
      }
      else if (status.MPI_TAG == ACK)
      {
        supplies_bought += receive_info.supplies_order;
        MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
      }
      else if (status.MPI_TAG == SUPPLY)
      {
        if(receive_info.supplies_order > 0)
        {
          if(receive_info.initial_message_from == rank)
          {
            if(msg_send)
            {
              if(receive_info.got_supplies == 1)
              {
                supplies_to_get -= receive_info.supplies_order;
                server_supplies += receive_info.supplies_order;
              }
              
              if(receive_info.message_from == right_neighbour)
              {
                receive_info.supplies_order = server_supplies - my_old_supplies;
                MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, SUPPLY_ACK, MPI_COMM_WORLD);
                msg_send = false; 
              }
              else
              {
                if(supplies_to_get == 0)
                {
                  receive_info.supplies_order = server_supplies - my_old_supplies;
                  MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, SUPPLY_ACK, MPI_COMM_WORLD); 
                  msg_send = false;  
                }
                          
              }
            }
            else
            {
              supplies_to_get = receive_info.supplies_order;
              msg_send = true;
              my_old_supplies = server_supplies;
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, SUPPLY, MPI_COMM_WORLD);
            }
          }
          else if(server_supplies <= 150)
          {
            receive_info.got_supplies = 0;
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, SUPPLY, MPI_COMM_WORLD);
          }
          else if(receive_info.supplies_order + 150 < server_supplies)
          {
            receive_info.got_supplies = 1;
            server_supplies -= receive_info.supplies_order;
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, receive_info.initial_message_from, SUPPLY, MPI_COMM_WORLD);
          }
          else if(150 < server_supplies <= receive_info.supplies_order)
          {
            receive_info.got_supplies = 1;
            int old_supplies_older = receive_info.supplies_order;
            receive_info.supplies_order = server_supplies - 150;
            server_supplies = 150;
            receive_info.message_from = rank;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, receive_info.initial_message_from, SUPPLY, MPI_COMM_WORLD);

            receive_info.supplies_order = old_supplies_older - receive_info.supplies_order;
            receive_info.got_supplies = 0;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, SUPPLY, MPI_COMM_WORLD);
          }  
        }
      }
      else if (status.MPI_TAG == EXTERNAL_SUPPLY)
      {
        if(leader_server == rank)
        {
          if(server_supplies < 150)
          {
            receive_info.id = 1;
            int get_supplies = 150 - server_supplies;
            if(receive_info.got_supplies - get_supplies >= 0)
            {
              receive_info.got_supplies -= get_supplies;
              server_supplies += get_supplies;
              receive_info.supplies_order = get_supplies;
            }
            else
            {
              receive_info.supplies_order = receive_info.got_supplies;
              server_supplies += receive_info.got_supplies;
              receive_info.got_supplies = 0;
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
            }
          }
          else
            receive_info.supplies_order = 0;


          if(receive_info.message_from == rank)
          {
            
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, EXTERNAL_SUPPLY, MPI_COMM_WORLD);
          }
          else if(receive_info.message_from == right_neighbour)
          {
            receive_info.id = 1;
            receive_info.message_from = rank;
            receive_info.supplies_order = receive_info.got_supplies;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
            server_supplies += receive_info.got_supplies;
          }
        }
        else
        {

          receive_info.message_from = rank;
          if(server_supplies < 150)
          {
            receive_info.supplies_order =  150 - server_supplies;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, leader_server, SUPPLY_REQUEST, MPI_COMM_WORLD);
            MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, leader_server, SUPPLY_REQUEST, MPI_COMM_WORLD, &status);
            server_supplies += receive_info.supplies_order;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, leader_server, ACK, MPI_COMM_WORLD);
          }
          else
            receive_info.supplies_order = 0;

          if(receive_info.got_supplies == 0)
          {
            receive_info.id = 1;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
          }
          else
          {
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, EXTERNAL_SUPPLY, MPI_COMM_WORLD);
          }
        }
      }
      else if (status.MPI_TAG == SUPPLY_REQUEST)
      {
        assert(leader_server == rank);
        if(receive_info.supplies_order < receive_info.got_supplies)
        {
          receive_info.got_supplies -= receive_info.supplies_order;
          MPI_Send(&receive_info, 1, MPI_INFO_TYPE, receive_info.message_from, SUPPLY_REQUEST, MPI_COMM_WORLD);
          MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, receive_info.message_from, ACK, MPI_COMM_WORLD, &status);
        }
        else
        {
          receive_info.supplies_order = receive_info.got_supplies;
          receive_info.got_supplies = 0;
          MPI_Send(&receive_info, 1, MPI_INFO_TYPE, receive_info.message_from, SUPPLY_REQUEST, MPI_COMM_WORLD);
          MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, receive_info.message_from, ACK, MPI_COMM_WORLD, &status);
        }
      }
      else if (status.MPI_TAG == REPORT)
      {
        if(leader_server == rank)
        {          
          if(receive_info.message_from == rank)
          {
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, REPORT, MPI_COMM_WORLD);
            for(int i = 0; i < my_clients.size(); i++)
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE, my_clients.at(i), REPORT, MPI_COMM_WORLD);
            for(int i = 0; i < my_clients.size(); i++)
            {
              MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, REPORT, MPI_COMM_WORLD, &status);
              get_all_supplies += receive_info.supplies_order;
            }
            cout << "REPORT: " << rank << " CLIENTS BOUGHT " << get_all_supplies << endl;
          }
         
          for(int i = 1; i < atoi(argv[1]); ++i)
          {
            MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, REPORT, MPI_COMM_WORLD, &status);
            get_all_supplies += receive_info.supplies_order;
          }
          receive_info.message_from = rank;
          receive_info.supplies_order = get_all_supplies;
          MPI_Send(&receive_info, 1, MPI_INFO_TYPE, 0, ACK, MPI_COMM_WORLD);
        }
        else
        {
          if(i_am_server)
          {
            receive_info.message_from = rank;
            get_all_supplies = 0;
            if(left_neighbour != leader_server)
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE, left_neighbour, REPORT, MPI_COMM_WORLD);

            for(int i = 0; i < my_clients.size(); i++)
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE, my_clients.at(i), REPORT, MPI_COMM_WORLD);
            
            for(int i = 0; i < my_clients.size(); i++)
            {
              MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, REPORT, MPI_COMM_WORLD, &status);
              get_all_supplies += receive_info.supplies_order;
            }
            receive_info.supplies_order = get_all_supplies;
            cout << "REPORT: " << rank << " CLIENTS BOUGHT " << get_all_supplies << endl;
            MPI_Send(&receive_info, 1, MPI_INFO_TYPE, leader_server, REPORT, MPI_COMM_WORLD);
          }
          else
          {
            if(my_clients.size() != 0)
            {
              receive_info.message_from = rank;
              for(int i = 0; i < my_clients.size(); i++)
                MPI_Send(&receive_info, 1, MPI_INFO_TYPE, my_clients.at(i), REPORT, MPI_COMM_WORLD);

              for(int i = 0; i < my_clients.size(); i++)
              {
                MPI_Recv(&receive_info, 1, MPI_INFO_TYPE, MPI_ANY_SOURCE, REPORT, MPI_COMM_WORLD, &status);
                get_all_supplies += receive_info.supplies_order;
              }

              if(supplies_bought == 0)
                receive_info.supplies_order = get_all_supplies;
              else
                 receive_info.supplies_order = get_all_supplies + supplies_bought;
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE, my_parent, REPORT, MPI_COMM_WORLD);

            }
            else
            {
              receive_info.message_from = rank;
              receive_info.supplies_order = supplies_bought;
              MPI_Send(&receive_info, 1, MPI_INFO_TYPE,my_parent, REPORT, MPI_COMM_WORLD);
            }
          }
        }
      }
      else if (status.MPI_TAG == TERMINATE)
      {
        MPI_Send(ack_buffer, 4, MPI_CHAR, 0, ACK, MPI_COMM_WORLD);
        break;
      }
    }
  } 

  MPI_Finalize();
 
  return 0;
}