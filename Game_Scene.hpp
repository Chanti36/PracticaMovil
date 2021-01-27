/*
 * INTRO SCENE
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

            // Estos typedefs pueden ayudar a hacer el código más compacto y claro:

            typedef std::shared_ptr < Sprite     >     Sprite_Handle;
            typedef std::list< Sprite_Handle     >     Sprite_List;
            typedef std::shared_ptr< Texture_2D  >     Texture_Handle;
            typedef std::map< Id, Texture_Handle >     Texture_Map;
            typedef basics::Graphics_Context::Accessor Context;


            /**
             * Representa el estado de la escena en su conjunto.
             */
            enum State
            {
                LOADING,
                RUNNING,
                ERROR
            };

            /**
             * Representa el estado del juego cuando el estado de la escena es RUNNING.
             */
            enum Gameplay_State
            {
                UNINITIALIZED,
                WAITING_TO_START,
                PLAYING,
            };

        private:

            /**
             * Array de estructuras con la información de las texturas (Id y ruta) que hay que cargar.
             */
            static struct   Texture_Data { Id id; const char * path; } textures_data[];

            //Array con los datos necesarios para los Invaders
            struct   Invaders
            {
                        Sprite_Handle invaders_Sprites;
                        float invader_xpos,invader_ypos;
                        bool invader_alive=true;
            }
            invaders[35];


            /**
             * Número de items que hay en el array textures_data.
             */
            static unsigned textures_count;

        private:


            State          state;                               ///< Estado de la escena.
            Gameplay_State gameplay;                            ///< Estado del juego cuando la escena está RUNNING.
            bool           suspended;                           ///< true cuando la escena está en segundo plano y viceversa.


            unsigned       canvas_width;                        ///< Ancho de la resolución virtual usada para dibujar.
            unsigned       canvas_height;                       ///< Alto  de la resolución virtual usada para dibujar.


            Texture_Map    textures;                            ///< Mapa  en el que se guardan shared_ptr a las texturas cargadas.
            Sprite_List    sprites;                             ///< Lista en la que se guardan shared_ptr a los sprites creados.

            Texture_Handle background;


            Sprite       *player;
            Sprite       *ammo;
            Sprite       *invader_ammo;

            float spritesize, spritescale;


            float x,x2,y, touch_x, touch_x2;                //Coordenadas del jugador y donde se toca la pantalla para el movimiento
            float distance;                                 //Distancia recorrida para el movimiento
            byte player_lifes;


            float ammo_speed;
            bool canshoot;

            float invaders_dir, invaders_speed;
            bool going_down, invader_shoot;
            int invader_shooting;

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

            void Collisions();
        };

    }

#endif
