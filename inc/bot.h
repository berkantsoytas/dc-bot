#ifndef BOT_H
#define BOT_H
void print_usage(void);
void on_ready(struct discord *client, const struct discord_ready *event);
void on_ping(struct discord *client, const struct discord_message *event);
void on_pong(struct discord *client, const struct discord_message *event);
#endif // !BOT_H