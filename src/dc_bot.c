#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "discord.h"
#include "log.h"
#include "bot.h"

int main(int argc, char *argv[])
{
    const char *config_file;
    if (argc > 1)
        config_file = argv[1];
    else
        config_file = "config.json";

    ccord_global_init();
    struct discord *client = discord_config_init(config_file);
    assert(NULL != client && "Could not initialize client");

    discord_set_on_ready(client, &on_ready);
    discord_set_on_guild_role_create(client, &log_on_role_create);
    discord_set_on_guild_role_update(client, &log_on_role_update);
    discord_set_on_guild_role_delete(client, &log_on_role_delete);

    discord_set_prefix(client, "!");

    discord_set_on_command(client, "ping", &on_ping);
    discord_set_on_command(client, "pong", &on_pong);
    discord_set_on_command(client, "role_create", &on_role_create);
    discord_set_on_command(client, "role_delete", &on_role_delete);
    discord_set_on_command(client, "role_add", &on_role_member_add);
    discord_set_on_command(client, "role_remove", &on_role_member_remove);
    discord_set_on_command(client, "role_list", &on_role_list);
    discord_set_on_command(client, "get_user", &on_member_get);

    print_usage();
    fgetc(stdin); // wait for input

    discord_run(client);

    discord_cleanup(client);
    ccord_global_cleanup();
}

void print_usage(void)
{
    printf("\n\nThis bot demonstrates a simple ping-pong response.\n"
           "1. Type 'pong' in chat\n"
           "2. Type 'ping' in chat\n"
           "\nTYPE ANY KEY TO START BOT\n");
}

void on_ready(struct discord *client, const struct discord_ready *event)
{
    log_info("PingPong-Bot succesfully connected to Discord as %s#%s!",
             event->user->username, event->user->discriminator);
}

void on_ping(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    struct discord_create_message params = {.content = "pong"};
    discord_create_message(client, event->channel_id, &params, NULL);
}

void on_pong(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    struct discord_create_message params = {.content = "ping"};
    discord_create_message(client, event->channel_id, &params, NULL);
}
