/*
 * GAME SCENE
 * Copyright © 2020+ Santiago_Gil_Moya
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * santigil009@gmail.com
 */

#ifndef GAME_SCENE_HEADER
#define GAME_SCENE_HEADER

    #include <map>
    #include <list>
    #include <memory>

    #include <basics/Canvas>
    #include <basics/Id>
    #include <basics/Scene>
    #include <basics/Texture_2D>
    #include <basics/Timer>

    #include "Sprite.hpp"

    namespace example
    {

        using basics::Id;
        using basics::Timer;
        using basics::Canvas;
        using basics::Texture_2D;

        class Game_Scene : public basics::Scene
        {

            typedef std::shared_ptr < Sprite     >     Sprite_Handle;
            typedef std::list< Sprite_Handle     >     Sprite_List;
            typedef std::shared_ptr< Texture_2D  >     Texture_Handle;
            typedef std::map< Id, Texture_Handle >     Texture_Map;
            typedef basics::Graphics_Context::Accessor Context;


            //Estados de la escena
            enum State
            {
                LOADING,
                RUNNING,
                ERROR
            };

           //Subestados de la escena en Running
            enum Gameplay_State
            {
                UNINITIALIZED,
                WAITING_TO_START,
                PLAYING,
            };

        private:

            //Info de las texturas que hay que cargar
            static struct   Texture_Data { Id id; const char * path; } textures_data[];

            //Array con los datos necesarios para los Invaders
            struct   Invaders
            {
                Sprite_Handle invaders_Sprites;
                float invader_xpos,invader_ypos;
                bool invader_alive=true;
            }invaders[35];



            static unsigned textures_count;                     //Numero de items del array textures_data

        private:


            State          state;                               // Estado de la escena.
            Gameplay_State gameplay;                            //Subestado de la escena en Running
            bool           suspended;                           //Bool para cuando está en segundo plano


            unsigned       canvas_width;                        //Ancho del canvas
            unsigned       canvas_height;                       //Alto del canvas


            Texture_Map    textures;                            // Mapa  en el que se guardan shared_ptr a las texturas cargadas.
            Sprite_List    sprites;                             // Lista en la que se guardan shared_ptr a los sprites creados.

            Texture_Handle background;                          //Texturas
            Texture_Handle life;

            Sprite      *player;                                //Punteros a los sprites
            Sprite      *ammo;
            Sprite      *invader_ammo;
            Sprite      *paused_base,*paused_presed;
            Sprite      *win_sprite,*gameover_sprite;
            Sprite      *playagain_base,*playagain_pressed;
            Sprite      *exit_base,*exit_pressed;


            float spritesize, spritescale;                      //tamaño y escala de los sprites para que se ajusten al tamaño de pantalla


            float x,x2,y, touch_x, touch_x2;                //Coordenadas del jugador y donde se toca la pantalla para el movimiento
            float distance;                                 //Distancia recorrida para el movimiento
            byte player_lifes;                              //Vidas del jugador
            float ammo_speed;                               //Velocidad de las balas
            bool canshoot;                                  //Determina si el jugador puede disparar o no


            float invaders_dir, invaders_speed;             //Direccion / Velocidad de los invaders
            bool going_down;                                //Determina si los invaders se mueven en Y
            int invader_shooting;                           //Determina el invader que dispara


            bool oncanvas,paused;                           //Determinar si el jugador toca el boton de pausa y si el juego entra en pausa
            bool gameover, win, finishedmenuinteraction;    //Determinar el estado de la escena al finalizar la partida
            int defeatedinvaders;                           //Numero de invaders derrotados


            Timer          timer;                           //Calcular intervalos de tiempo

        public:

            //Inicializa los atributos al crear la escena de 0
            Game_Scene();

            //Resolucion virtual de la escena
            basics::Size2u get_view_size () override
            {
                return { canvas_width, canvas_height };
            }

            //inicializa los atributos al iniciar la esccena
            bool initialize () override;

            //Si pasa a segundo plano
            void suspend () override;

            //Vuelta a primer plano
            void resume () override;

            //Eventos al tocar la pantalla
            void handle (basics::Event & event) override;

           //Actualizar la escena constantemente
            void update (float time) override;

            //Dibujar los frames
            void render (Context & context) override;

        private:

            //Cargar las texturas / frame para poder pausarlo si se pone en segundo plano
            void load_textures ();

            //Crear los sprites basados en las texturas
            void create_sprites ();

            //Reiniciar el juego
            void restart_game ();

            //Empezar a jugar
            void start_playing ();

            //Actualiza el estado del juego mientras se ejecuta
            void run_simulation (float time);


            //Dibuja la textura para el estado de carga
            void render_loading (Canvas & canvas);

            //Dibuja la escena del juego
            void render_playfield (Canvas & canvas);

            //Disparar un proyectil
            void Shoot();

            //Controlador de los invaders
            void InvaderAI();

            //Controlador de las colisiones
            void Collisions();

            //Controlador del final de la partida
            void GameOver();
        };

    }

#endif
