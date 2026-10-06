
#### Event Payload {#event_payload}

`Data` and `Data2` fields are in the format (bits 0-31, bits 32-63).  
Unused is used if both low and high bits are unused.
| Event | Data | Data2
|---|---|---
| PAL_EVENT_TYPE_WINDOW_CLOSE | unused | window |
| PAL_EVENT_TYPE_WINDOW_SIZE | width, height | window |
| PAL_EVENT_TYPE_WINDOW_MOVE | x, y | window |
| PAL_EVENT_TYPE_WINDOW_STATE | state, unused | window |
| PAL_EVENT_TYPE_WINDOW_FOCUS | focus, unused | window |
| PAL_EVENT_TYPE_WINDOW_VISIBILITY | visibility, unused | window |
| PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN | unused | window |
| PAL_EVENT_TYPE_WINDOW_MODAL_END | unused | window |
| PAL_EVENT_TYPE_MONITOR_DPI_CHANGED | dpi, unused | window |
| PAL_EVENT_TYPE_MONITOR_LIST_CHANGED | unused | window |
| PAL_EVENT_TYPE_KEYDOWN | keycode, scancode | window |
| PAL_EVENT_TYPE_KEYREPEAT | keycode, scancode | window |
| PAL_EVENT_TYPE_KEYUP | keycode, scancode | window |
| PAL_EVENT_TYPE_MOUSE_BUTTONDOWN | button, serial | window |
| PAL_EVENT_TYPE_MOUSE_BUTTONUP | button, serial | window |
| PAL_EVENT_TYPE_MOUSE_MOVE | x, y | window |
| PAL_EVENT_TYPE_MOUSE_DELTA | dx, dy | window |
| PAL_EVENT_TYPE_MOUSE_WHEEL | dx, dy | window |
| PAL_EVENT_TYPE_USER | user defined | user defined |
| PAL_EVENT_TYPE_KEYCHAR | codepoint, unused | window |
| PAL_EVENT_TYPE_WINDOW_DECORATION_MODE | decoration mode, unused | window |