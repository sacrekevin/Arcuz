#include "NetworkClient.h"
#include <iostream>
#include <functional>
#include <SFML/Network/Packet.hpp>
#include "data/NetworkData.h"
#include <experimental/filesystem>
#include "../../viewer/texture/CharacterTexture.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

NetworkClient::NetworkClient(int width, int height, std::string ip, unsigned int port) : Controller(width, height), ip(ip){
  this->port = port;
  this->connectGame();
}

void NetworkClient::communicate(sf::Packet &packet){
  this->connectServer();
  this->send(packet);
}

bool NetworkClient::connectServer(){
  if(this->socket.connect(this->ip, this->port) == sf::Socket::Done){
    std::cout << "connection succeed" << std::endl;
    return true;
  }
  std::cout << "connection failed" << std::endl;
  return false;
}

void NetworkClient::send(sf::Packet &packet){
  if(!this->socket.send(packet) == sf::Socket::Status::Done){
    std::cout << "packet send failed" << std::endl;
  }
}

void NetworkClient::connectGame(){
  sf::Packet packet;

  packet << (uint32_t)Action::connect;
  packet << this->model->getMainCharacter().getName();
  packet << (uint32_t)model->getMainCharacter().getType();

  this->communicate(packet);
}

void NetworkClient::disconnectGame(){
  sf::Packet packet;

  packet << (uint32_t)Action::disconnect;
  packet << this->model->getMainCharacter().getName();

  this->communicate(packet);
  std::cout << "deconnection du serveur" << std::endl;
}

void NetworkClient::updateCLient(){
  sf::Packet packet;

  if(this->socket.receive(packet) == sf::Socket::Done){
    unsigned int size;
    std::vector<EntityDrawable> entities;
    EntityDrawable entity;

    packet >> size;

    for(unsigned int i = 0; i < size; i++){
      packet >> entity;
      entities.push_back(entity);
    }
    //this->model->setEntities(entities);
  }
}

void NetworkClient::start(){
  while(this->running){
    this->checkEvents();
    this->model->update();
    this->model->render();
    sf::Thread thread(&NetworkClient::updateCLient, this);
    thread.launch();
  }
}

void NetworkClient::checkEvents(){
    sf::Event event;
    while (this->window.pollEvent(event)) {

     if (event.type == sf::Event::KeyPressed){ //ANY KEY PRESSED

       if(event.key.shift){ //SHIFT PRESSED
         if(event.key.code == sf::Keyboard::Right){
           this->model->getMainCharacter().run(Direction::right);
         }else if(event.key.code == sf::Keyboard::Left){
           this->model->getMainCharacter().run(Direction::left);
         }else if(event.key.code == sf::Keyboard::Up){
           this->model->getMainCharacter().run(Direction::up);
         }else if(event.key.code == sf::Keyboard::Down){
           this->model->getMainCharacter().run(Direction::down);
         }
       }

       else if (event.key.code == sf::Keyboard::Escape){ //CASE ELSE
         this->disconnectGame();
         this->running = false;
       }else if(event.key.code == sf::Keyboard::Right){
         this->model->getMainCharacter().walk(Direction::right);
       }else if(event.key.code == sf::Keyboard::Left){
         this->model->getMainCharacter().walk(Direction::left);
       }else if(event.key.code == sf::Keyboard::Up){
         this->model->getMainCharacter().walk(Direction::up);
       }else if(event.key.code == sf::Keyboard::Down){
         this->model->getMainCharacter().walk(Direction::down);
       }else{
         if(this->model->getMainCharacter().getSpeed() != 0){
           this->model->getMainCharacter().stop();
         }
       }
     }else{ // IF NO KEY PRESSED
       this->model->getMainCharacter().stop();
     }

     if(event.type == sf::Event::Closed){ // IF WINDOWS CLOSE
       this->disconnectGame();
       this->closeGame();
     }
   }
}
