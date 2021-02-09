/*
 * MENU SCENE
 * Copyright © 2020+ Santiago_Gil_Moya
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * santigil009@gmail.com
 */

#include "Menu_Scene.hpp"
#include "Game_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

using namespace basics;
using namespace std;

namespace example
{

    Menu_Scene::Menu_Scene()
    {
        state         = LOADING;
        suspended     = true;

        canvas_width = 720;
        canvas_height = 1280;
        bottomover=0;
    }

    // ---------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {
        oncredits=false;
        onhelp   =false;

        return true;
    }

    // ---------------------------------------------------------------------------------------------



    void Menu_Scene::handle (basics::Event & event)
    {
        if (state == READY)                     // Se descartan los eventos cuando la escena está LOADING
        {
            switch (event.id)
            {
                case ID(touch-started):         // El usuario toca la pantalla
                case ID(touch-moved):
                {
                    // Se determina qué opción se ha tocado:

                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };


                    //PLAY
                    if(touch_location.coordinates.y() < playpos + (play_base->get_height()*.5f) &&
                       touch_location.coordinates.y() > playpos - (play_base->get_height()*.5f))
                    {
                        if(touch_location.coordinates.x() < canvas_width*.5f + (play_base->get_width()*.5f) &&
                           touch_location.coordinates.x() > canvas_width*.5f - (play_base->get_width()*.5f))
                        {
                            bottomover=1;
                        }
                    }
                    //HELP
                    else if(touch_location.coordinates.y() < helppos + (help_base->get_height()*.5f) &&
                            touch_location.coordinates.y() > helppos - (help_base->get_height()*.5f))
                    {
                        if(touch_location.coordinates.x() < canvas_width*.5f + (help_base->get_width()*.5f) &&
                           touch_location.coordinates.x() > canvas_width*.5f - (help_base->get_width()*.5f))
                        {
                            bottomover=2;
                        }
                    }
                    //CREDITS
                    else if(touch_location.coordinates.y() < creditspos+ (credits_base->get_height()*.5f) &&
                       touch_location.coordinates.y() > creditspos - (credits_base->get_height()*.5f))
                    {
                        if(touch_location.coordinates.x() < canvas_width*.5f + (credits_base->get_width()*.5f) &&
                           touch_location.coordinates.x() > canvas_width*.5f - (credits_base->get_width()*.5f))
                        {
                            bottomover=3;
                        }
                    }
                    else
                    {
                        bottomover=0;
                    }



                    break;
                }

                case ID(touch-ended):           // El usuario deja de tocar la pantalla
                {
                    Point2f touch_ended_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                    if(oncredits==false && onhelp==false)
                    {
                        //PLAY
                        if(touch_ended_location.coordinates.y() < playpos + (play_base->get_height()*.5f) &&
                           touch_ended_location.coordinates.y() > playpos - (play_base->get_height()*.5f))
                        {
                            if(touch_ended_location.coordinates.x() < canvas_width*.5f + (play_base->get_width()*.5f) &&
                               touch_ended_location.coordinates.x() > canvas_width*.5f - (play_base->get_width()*.5f))
                            {
                                director.run_scene (shared_ptr< Scene >(new Game_Scene));
                            }
                        }
                        //HELP
                        if(touch_ended_location.coordinates.y() < helppos + (help_base->get_height()*.5f) &&
                           touch_ended_location.coordinates.y() > helppos - (help_base->get_height()*.5f))
                        {
                            if(touch_ended_location.coordinates.x() < canvas_width*.5f + (help_base->get_width()*.5f) &&
                               touch_ended_location.coordinates.x() > canvas_width*.5f - (help_base->get_width()*.5f))
                            {
                                onhelp=true;
                            }
                        }
                        //CREDITS
                        if(touch_ended_location.coordinates.y() < creditspos+ (credits_base->get_height()*.5f) &&
                           touch_ended_location.coordinates.y() > creditspos - (credits_base->get_height()*.5f))
                        {
                            if(touch_ended_location.coordinates.x() < canvas_width*.5f + (credits_base->get_width()*.5f) &&
                               touch_ended_location.coordinates.x() > canvas_width*.5f - (credits_base->get_width()*.5f))
                            {
                                oncredits=true;
                            }
                        }
                    }
                    else
                    {
                        onhelp   =false;
                        oncredits=false;
                    }

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::update (float time)
    {
        switch (state)
        {
            case LOADING: load();
                break;
            case READY: run();
                break;
        }

    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended && state == READY)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                canvas->clear ();

                canvas->fill_rectangle ({ canvas_width*.5f,canvas_height*.5f },                                     //FONDO
                                        { canvas_width*1.0f,canvas_height*1.0f },
                bg_texture.get());//CAMBIAR PARA QUE SE AJUSTE AL ALTO DE CADA PANTALLA EN FUNCIÓN DEL ANCHO


                canvas->fill_rectangle                                                               //TITLE
                        ({ canvas_width*.5f, canvas_height*.75f },
                         {title_texture->get_width(),title_texture->get_height() },
                title_texture.get());



                if(onhelp)
                {
                    canvas->fill_rectangle
                            ({ canvas_width*.5f, canvas_height*.5f },
                             {canvas_width*1.0f,(help->get_height()*(canvas_width/help->get_width())) },
                             help.get());
                }
                else if(oncredits)
                {
                    canvas->fill_rectangle
                            ({ canvas_width*.5f, canvas_height*.5f },
                             {canvas_width*1.0f,(help->get_height()*(canvas_width/credits->get_width())) },
                             credits.get());
                }
                else if(bottomover==1)
                {
                    canvas->fill_rectangle
                            ({ canvas_width*.5f, canvas_height*.5f },
                             {play_base->get_width(),play_base->get_height() },
                             play_pressed.get());
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*1.5f) },
                             {help_base->get_width(),help_base->get_height() },
                             help_base.get());
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*3) },
                             {credits_base->get_width(),credits_base->get_height() },
                             credits_base.get());
                }
                else if(bottomover==2)
                {
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, canvas_height*.5f },
                             {play_base->get_width(),play_base->get_height() },
                             play_base.get());
                    canvas->fill_rectangle
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*1.5f) },
                             {help_base->get_width(),help_base->get_height() },
                             help_pressed.get());
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*3) },
                             {credits_base->get_width(),credits_base->get_height() },
                             credits_base.get());
                }
                else if(bottomover==3)
                {
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, canvas_height*.5f },
                             {play_base->get_width(),play_base->get_height() },
                             play_base.get());
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*1.5f) },
                             {help_base->get_width(),help_base->get_height() },
                             help_base.get());
                    canvas->fill_rectangle
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*3) },
                             {credits_base->get_width(),credits_base->get_height() },
                             credits_pressed.get());
                }
                else if(bottomover==0)
                {
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, canvas_height*.5f },
                             {play_base->get_width(),play_base->get_height() },
                             play_base.get());
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*1.5f) },
                             {help_base->get_width(),help_base->get_height() },
                             help_base.get());
                    canvas->fill_rectangle                                                               //TITLE
                            ({ canvas_width*.5f, (canvas_height*.5f - play_base->get_height()*3) },
                             {credits_base->get_width(),credits_base->get_height() },
                             credits_base.get());
                }

            }
        }
    }

    void Menu_Scene:: load()
    {
        if (!suspended)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

             if (context)
             {
                // Se cargan las texturas
                 bg_texture     = Texture_2D::create (ID(bg),       context, "menu-scene/menuscene.png");
                 title_texture  = Texture_2D::create (ID(title),    context, "menu-scene/title.png");
                 play_base      = Texture_2D::create (ID(play_b),   context, "menu-scene/play0.png");
                 play_pressed   = Texture_2D::create (ID(play_p),   context, "menu-scene/play.png");
                 help_base      = Texture_2D::create (ID(help_b),   context, "menu-scene/help0.png");
                 help_pressed   = Texture_2D::create (ID(help_p),   context, "menu-scene/help.png");
                 credits_base   = Texture_2D::create (ID(credit_b), context, "menu-scene/credits0.png");
                 credits_pressed= Texture_2D::create (ID(credit_p), context, "menu-scene/credits.png");
                 credits        = Texture_2D::create (ID(credits),  context, "menu-scene/credits1.png");
                 help           = Texture_2D::create (ID(help),     context, "menu-scene/help1.png");


                context->add(bg_texture);
                context->add(title_texture);
                context->add(play_base);
                context->add(play_pressed);
                context->add(help_base);
                context->add(help_pressed);
                context->add(credits_base);
                context->add(credits_pressed);
                context->add(credits);
                context->add(help);



                 playpos = canvas_height*.5f;
                 helppos=canvas_height*.5f - play_base->get_height()*1.5f;
                 creditspos=canvas_height*.5f - play_base->get_height()*3;

                state = READY;
             }
        }
    }

    void Menu_Scene:: run()
    {

        if(oncredits)
        {
            bottomover=0;
        }
        else if(onhelp)
        {
            bottomover=0;
        }
    }

}