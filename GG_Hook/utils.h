#pragma once

void BindCrtHandlesToStdHandles(bool bindStdIn, bool bindStdOut, bool bindStdErr);
void clear_screen(char fill = ' ');
void MsgBoxInfo(LPCSTR text);