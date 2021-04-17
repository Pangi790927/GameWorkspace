#ifndef GUI_H
#define GUI_H

#include "Texture.h"
#include "imgui.h"

#define IMGUI_R_KEY(x) (x > 0xff ? ((x & 0xff) + 0x100) : x)


struct ImGuiCtx {
	Texture glfont;
	GLuint       g_GlVersion = 200;
	char         g_GlslVersionString[32] = "";
	GLuint       g_ShaderHandle = 0;
	GLint        g_VertHandle = 0;
	GLint        g_FragHandle = 0;
	GLint        g_AttribLocationTex = 0;
	GLint        g_AttribLocationProjMtx = 0;
	GLuint       g_AttribLocationVtxPos = 0;
	GLuint       g_AttribLocationVtxUV = 0;
	GLuint       g_AttribLocationVtxColor = 0;
	unsigned int g_VboHandle = 0;
	unsigned int g_ElementsHandle = 0;
};
inline ImGuiCtx imgui_ctx;

inline int init_imgui(OpenglWindow *window);
inline int uninit_imgui();

inline int process_events_imgui(OpenglWindow *window);
inline int predraw_imgui(OpenglWindow *window);
inline int render_imgui();


inline int predraw_imgui(OpenglWindow *window) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)window->width, (float)window->height);

/* TODO: update cursor */
	// ImGuiIO& io = ImGui::GetIO();
 //    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
 //        return;

 //    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
 //    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
 //    {
 //        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
 //        al_set_mouse_cursor(g_Display, g_MouseCursorInvisible);
 //    }
 //    else
 //    {
 //        ALLEGRO_SYSTEM_MOUSE_CURSOR cursor_id =
    	// ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
 //        switch (imgui_cursor)
 //        {
   //      case ImGuiMouseCursor_TextInput:
   // 			cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT; break;
   //      case ImGuiMouseCursor_ResizeAll:
 		// 	cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE; break;
   //      case ImGuiMouseCursor_ResizeNS:
			// cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N; break;
   //      case ImGuiMouseCursor_ResizeEW:
 		// 	cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E; break;
   //      case ImGuiMouseCursor_ResizeNESW:
 		// 	cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE; break;
   //      case ImGuiMouseCursor_ResizeNWSE:
 		// 	cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW; break;
   //      case ImGuiMouseCursor_NotAllowed:
 		// 	cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE; break;
 //        }
 //        al_set_system_mouse_cursor(g_Display, cursor_id);
 //    }

	ImGui::NewFrame();
	return 0;
}

inline int process_events_imgui(OpenglWindow *window) {
	auto ks = [&](int key) { return window->keyboard.getKeyState(key); };
	ImGuiIO& io = ImGui::GetIO();

	io.MouseWheel = window->mouse.scroll;
	io.MouseWheelH = window->mouse.scrollH;
	io.MousePos = ImVec2(window->mouse.x, window->mouse.y);
	io.MouseDown[0] = window->mouse.lmb;
	io.MouseDown[1] = window->mouse.rmb;
	io.MouseDown[2] = window->mouse.mmb;
	io.KeyCtrl = ks(window->keyboard.L_CTRL) || ks(window->keyboard.R_CTRL);
	io.KeyShift = ks(window->keyboard.L_SHIFT) || ks(window->keyboard.R_SHIFT);
	io.KeyAlt = ks(window->keyboard.L_ALT) || ks(window->keyboard.R_ALT);
	io.KeySuper = ks(window->keyboard.WINKEY);
	KeyEvent event;
	while (!window->keyboard.queEmpty()) {
		event = window->keyboard.popEvent();
		if (event.press && event.key >= 32 && event.key < 128)
			io.AddInputCharacter(event.key);
		io.KeysDown[IMGUI_R_KEY(event.key)] = event.press;
	}
	return 0;
}

static bool CheckProgram(GLuint handle, const char* desc)
{
	GLint status = 0, log_length = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if ((GLboolean)status == GL_FALSE)
		fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects:"
				"failed to link %s! (with GLSL '%s')\n", desc,
				imgui_ctx.g_GlslVersionString);
	if (log_length > 1)
	{
		ImVector<char> buf;
		buf.resize((int)(log_length + 1));
		glGetProgramInfoLog(handle, log_length, NULL, (GLchar*)buf.begin());
		fprintf(stderr, "%s\n", buf.begin());
	}
	return (GLboolean)status == GL_TRUE;
}

static bool CheckShader(GLuint handle, const char* desc)
{
	GLint status = 0, log_length = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if ((GLboolean)status == GL_FALSE)
		fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects:"
				"failed to compile %s!\n", desc);
	if (log_length > 1)
	{
		ImVector<char> buf;
		buf.resize((int)(log_length + 1));
		glGetShaderInfoLog(handle, log_length, NULL, (GLchar*)buf.begin());
		fprintf(stderr, "%s\n", buf.begin());
	}
	return (GLboolean)status == GL_TRUE;
}

inline int init_imgui(OpenglWindow *window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendPlatformName = io.BackendRendererName = "imgui_own_impl";

	// TODO: fill those defs
	printf("left: %d\n", IMGUI_R_KEY(window->keyboard.ARROW_LEFT));
	printf("right: %d\n", IMGUI_R_KEY(window->keyboard.ARROW_RIGHT));
	io.KeyMap[ImGuiKey_Tab] =         IMGUI_R_KEY(window->keyboard.TAB);
	io.KeyMap[ImGuiKey_LeftArrow] =   IMGUI_R_KEY(window->keyboard.ARROW_LEFT);
	io.KeyMap[ImGuiKey_RightArrow] =  IMGUI_R_KEY(window->keyboard.ARROW_RIGHT);
	io.KeyMap[ImGuiKey_UpArrow] =     IMGUI_R_KEY(window->keyboard.ARROW_UP);
	io.KeyMap[ImGuiKey_DownArrow] =   IMGUI_R_KEY(window->keyboard.ARROW_DOWN);
	io.KeyMap[ImGuiKey_PageUp] =      IMGUI_R_KEY(window->keyboard.PAGE_UP);
	io.KeyMap[ImGuiKey_PageDown] =    IMGUI_R_KEY(window->keyboard.PAGE_DOWN);
	io.KeyMap[ImGuiKey_Home] =        IMGUI_R_KEY(window->keyboard.HOME);
	io.KeyMap[ImGuiKey_End] =         IMGUI_R_KEY(window->keyboard.END);
	io.KeyMap[ImGuiKey_Insert] =      IMGUI_R_KEY(window->keyboard.INSERT);
	io.KeyMap[ImGuiKey_Delete] =      IMGUI_R_KEY(window->keyboard.DELETE);
	io.KeyMap[ImGuiKey_Backspace] =   IMGUI_R_KEY(window->keyboard.BACKSPACE);
	io.KeyMap[ImGuiKey_Space] =       IMGUI_R_KEY(window->keyboard.SPACE);
	io.KeyMap[ImGuiKey_Enter] =       IMGUI_R_KEY(window->keyboard.ENTER);
	io.KeyMap[ImGuiKey_Escape] =      IMGUI_R_KEY(window->keyboard.ESC);
	io.KeyMap[ImGuiKey_KeyPadEnter] = IMGUI_R_KEY(window->keyboard.NUM_ENTER);
	io.KeyMap[ImGuiKey_A] =           'a';
	io.KeyMap[ImGuiKey_C] =           'c';
	io.KeyMap[ImGuiKey_V] =           'v';
	io.KeyMap[ImGuiKey_X] =           'x';
	io.KeyMap[ImGuiKey_Y] =           'y';
	io.KeyMap[ImGuiKey_Z] =           'z';
	io.MousePos = ImVec2(0, 0);

	/* TODO: Implement those */
	// io.SetClipboardTextFn = ImGui_ImplAllegro5_SetClipboardText;
	// io.GetClipboardTextFn = ImGui_ImplAllegro5_GetClipboardText;
	// io.ClipboardUserData = NULL;

	const char *glsl_version = "#version 100";
	IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(
			imgui_ctx.g_GlslVersionString));
	strcpy(imgui_ctx.g_GlslVersionString, glsl_version);
	strcat(imgui_ctx.g_GlslVersionString, "\n");

	// Backup GL state
	GLint last_texture, last_array_buffer;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
	GLint last_vertex_array;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#endif

	const GLchar* vertex_shader_glsl_120 =
		"uniform mat4 ProjMtx;\n"
		"attribute vec2 Position;\n"
		"attribute vec2 UV;\n"
		"attribute vec4 Color;\n"
		"varying vec2 Frag_UV;\n"
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    Frag_UV = UV;\n"
		"    Frag_Color = Color;\n"
		"    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader_glsl_120 =
		"#ifdef GL_ES\n"
		"    precision mediump float;\n"
		"#endif\n"
		"uniform sampler2D Texture;\n"
		"varying vec2 Frag_UV;\n"
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
		"}\n";

	// Select shaders matching our GLSL versions
	const GLchar* vertex_shader = NULL;
	const GLchar* fragment_shader = NULL;
	vertex_shader = vertex_shader_glsl_120;
	fragment_shader = fragment_shader_glsl_120;


	// Create shaders
	const GLchar* vertex_shader_with_version[2] = {
			imgui_ctx.g_GlslVersionString, vertex_shader };
	imgui_ctx.g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(imgui_ctx.g_VertHandle, 2, vertex_shader_with_version, NULL);
	glCompileShader(imgui_ctx.g_VertHandle);
	CheckShader(imgui_ctx.g_VertHandle, "vertex shader");

	const GLchar* fragment_shader_with_version[2] = {
			imgui_ctx.g_GlslVersionString, fragment_shader };
	imgui_ctx.g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(imgui_ctx.g_FragHandle, 2,
			fragment_shader_with_version, NULL);
	glCompileShader(imgui_ctx.g_FragHandle);
	CheckShader(imgui_ctx.g_FragHandle, "fragment shader");

	imgui_ctx.g_ShaderHandle = glCreateProgram();
	glAttachShader(imgui_ctx.g_ShaderHandle, imgui_ctx.g_VertHandle);
	glAttachShader(imgui_ctx.g_ShaderHandle, imgui_ctx.g_FragHandle);
	glLinkProgram(imgui_ctx.g_ShaderHandle);
	CheckProgram(imgui_ctx.g_ShaderHandle, "shader program");

	imgui_ctx.g_AttribLocationTex = glGetUniformLocation(
			imgui_ctx.g_ShaderHandle, "Texture");
	imgui_ctx.g_AttribLocationProjMtx = glGetUniformLocation(
			imgui_ctx.g_ShaderHandle, "ProjMtx");
	imgui_ctx.g_AttribLocationVtxPos = (GLuint)glGetAttribLocation(
			imgui_ctx.g_ShaderHandle, "Position");
	imgui_ctx.g_AttribLocationVtxUV = (GLuint)glGetAttribLocation(
			imgui_ctx.g_ShaderHandle, "UV");
	imgui_ctx.g_AttribLocationVtxColor = (GLuint)glGetAttribLocation(
			imgui_ctx.g_ShaderHandle, "Color");

	// Create buffers
	glGenBuffers(1, &imgui_ctx.g_VboHandle);
	glGenBuffers(1, &imgui_ctx.g_ElementsHandle);

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
	glBindVertexArray(last_vertex_array);
#endif

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	imgui_ctx.glfont.alocateSpace(width, height, 4);
	memcpy(imgui_ctx.glfont.textureData.get(), pixels,
			sizeof(int) * width * height);
	imgui_ctx.glfont.generateOpenGLTexture();

	io.Fonts->SetTexID(NULL);
	return 0;
}

inline int uninit_imgui() {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->SetTexID(NULL);

	if (imgui_ctx.g_VboHandle) {
		glDeleteBuffers(1, &imgui_ctx.g_VboHandle);
		imgui_ctx.g_VboHandle = 0;
	}
	if (imgui_ctx.g_ElementsHandle) {
		glDeleteBuffers(1, &imgui_ctx.g_ElementsHandle);
		imgui_ctx.g_ElementsHandle = 0;
	}
	if (imgui_ctx.g_ShaderHandle && imgui_ctx.g_VertHandle) {
		glDetachShader(imgui_ctx.g_ShaderHandle, imgui_ctx.g_VertHandle);
	}
	if (imgui_ctx.g_ShaderHandle && imgui_ctx.g_FragHandle) {
		glDetachShader(imgui_ctx.g_ShaderHandle, imgui_ctx.g_FragHandle);
	}
	if (imgui_ctx.g_VertHandle) {
		glDeleteShader(imgui_ctx.g_VertHandle);
		imgui_ctx.g_VertHandle = 0;
	}
	if (imgui_ctx.g_FragHandle) {
		glDeleteShader(imgui_ctx.g_FragHandle);
		imgui_ctx.g_FragHandle = 0;
	}
	if (imgui_ctx.g_ShaderHandle) {
		glDeleteProgram(imgui_ctx.g_ShaderHandle);
		imgui_ctx.g_ShaderHandle = 0;
	}

	imgui_ctx.glfont.destroyTexture();
	return 0;
}

inline int render_imgui() {
	ImGui::Render();
	auto draw_data = ImGui::GetDrawData();
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		return 0;

	int fb_width = (int)(draw_data->DisplaySize.x *
			draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y *
			draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
	    return 0;

	// Backup GL state
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE,
			(GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLuint last_program; glGetIntegerv(GL_CURRENT_PROGRAM,
			(GLint*)&last_program);
	GLuint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D,
			(GLint*)&last_texture);
	GLuint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING,
			(GLint*)&last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
    GLuint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING,
    		(GLint*)&last_vertex_array_object);
#endif
#ifdef GL_POLYGON_MODE
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE,
			last_polygon_mode);
#endif
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX,
			last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB,
			(GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB,
			(GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA,
			(GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA,
			(GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB,
			(GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA,
			(GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup desired GL state
	GLuint vertex_array_object = 0;
#ifndef IMGUI_IMPL_OPENGL_ES2
	glGenVertexArrays(1, &vertex_array_object);
#endif
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
			GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	const float ortho_projection[4][4] =
	{
	    { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
	    { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
	    { 0.0f,         0.0f,        -1.0f,   0.0f },
	    { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
	};
	glUseProgram(imgui_ctx.g_ShaderHandle);
	glUniform1i(imgui_ctx.g_AttribLocationTex, 0);
	glUniformMatrix4fv(imgui_ctx.g_AttribLocationProjMtx, 1,
		GL_FALSE, &ortho_projection[0][0]);


	(void)vertex_array_object;
#ifndef IMGUI_IMPL_OPENGL_ES2
    glBindVertexArray(vertex_array_object);
#endif

	// Bind vertex/index buffers and setup attributes for ImDrawVert
	glBindBuffer(GL_ARRAY_BUFFER, imgui_ctx.g_VboHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgui_ctx.g_ElementsHandle);
	glEnableVertexAttribArray(imgui_ctx.g_AttribLocationVtxPos);
	glEnableVertexAttribArray(imgui_ctx.g_AttribLocationVtxUV);
	glEnableVertexAttribArray(imgui_ctx.g_AttribLocationVtxColor);
	glVertexAttribPointer(imgui_ctx.g_AttribLocationVtxPos,   2,
			GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert),
			(GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(imgui_ctx.g_AttribLocationVtxUV,    2,
			GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert),
			(GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(imgui_ctx.g_AttribLocationVtxColor, 4,
			GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(ImDrawVert),
			(GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;
	ImVec2 clip_scale = draw_data->FramebufferScale;

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
	    const ImDrawList* cmd_list = draw_data->CmdLists[n];

	    // Upload vertex/index buffers
	    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size *
	    		(int)sizeof(ImDrawVert),
	    		(const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	    		(GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx),
	    		(const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

	    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
	    {
	        const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
	        // Project scissor/clipping rectangles into framebuffer space
	        ImVec4 clip_rect;
	        clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
	        clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
	        clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
	        clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

	        if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z
	        		>= 0.0f && clip_rect.w >= 0.0f)
	        {
	            // Apply scissor/clipping rectangle
	            glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w),
	            		(int)(clip_rect.z - clip_rect.x),
	            		(int)(clip_rect.w - clip_rect.y));

	            // Bind texture, Draw
	            imgui_ctx.glfont.bind();
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
	            if (g_GlVersion >= 320)
	                glDrawElementsBaseVertex(GL_TRIANGLES,
	                		(GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ?
	                		GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
	                		(void*)(intptr_t)(pcmd->IdxOffset *
	                		sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
	            else
#endif
	            glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
	            		sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT :
	            		GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset *
	            		sizeof(ImDrawIdx)));
	        }
	    }
	}

    // Destroy the temporary VAO
#ifndef IMGUI_IMPL_OPENGL_ES2
	glDeleteVertexArrays(1, &vertex_array_object);
#endif

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glActiveTexture(last_active_texture);
#ifndef IMGUI_IMPL_OPENGL_ES2
	glBindVertexArray(last_vertex_array_object);
#endif
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb,
			last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	if (last_enable_cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	if (last_enable_depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	if (last_enable_stencil_test)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	if (last_enable_scissor_test)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);

#ifdef GL_POLYGON_MODE
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2],
			(GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1],
			(GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
	return 0;
}

#endif