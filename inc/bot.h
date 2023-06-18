#ifndef BOT_H
#define BOT_H

#include <stdio.h>
#include <stdlib.h>
#include "discord.h"
#include "log.h"

void print_usage(void);
void on_ready(struct discord *client, const struct discord_ready *event);
void on_ping(struct discord *client, const struct discord_message *event);
void on_pong(struct discord *client, const struct discord_message *event);

// guilds
void log_on_role_create(struct discord *client, const struct discord_guild_role_create *event)
{
    log_warn("Role created: %s (%" PRIu64 ")", event->role->name, event->role->id);
}

void log_on_role_update(struct discord *client, const struct discord_guild_role_update *event)
{
    log_warn("Role updated: %s (%" PRIu64 ")", event->role->name, event->role->id);
}

void log_on_role_delete(struct discord *client, const struct discord_guild_role_delete *event)
{
    log_warn("Role deleted: %" PRIu64, event->role_id);
}

void on_role_create(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    char role_name[128];

    scanf(event->content, "%s", role_name);

    if (!*role_name)
    {
        log_error("Could create role: no name provided");
        return;
    }

    struct discord_create_guild_role params = {.name = role_name};
    discord_create_guild_role(client, event->guild_id, &params, NULL);
}

void on_role_delete(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    u64snowflake role_id = 0;

    scanf(event->content, "%" SCNu64, &role_id);

    if (!role_id)
    {
        log_error("Invalid format for `prefix.role_delete <role_id>`");
        return;
    }

    struct discord_delete_guild_role params = {.reason = "Self-destruct"};
    discord_delete_guild_role(client, event->guild_id, role_id, &params, NULL);
}

void on_role_member_add(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    u64snowflake user_id = 0, role_id = 0;

    sscanf(event->content, "%" SCNu64 " %" SCNu64, &user_id, &role_id);

    if (!user_id || !role_id)
    {
        log_error("Invalid format for `prefix.role_member_add <user_id> <role_id>`");
        return;
    }

    struct discord_add_guild_member_role params =
        {
            .reason = "You specifically asked for this role",
        };

    discord_add_guild_member_role(client, event->guild_id, user_id, role_id, &params, NULL);

    log_info("Added role %" PRIu64 " to user %" PRIu64, role_id, user_id);
}

void on_role_member_remove(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    u64snowflake user_id = 0, role_id = 0;

    scanf(event->content, "%" SCNu64 " %" SCNu64, &user_id, &role_id);

    if (!user_id || !role_id)
    {
        log_error("Invalid format for `prefix.role_member_remove <user_id> <role_id>`");
        return;
    }

    struct discord_remove_guild_member_role params = {.reason = "You don't need this role"};

    discord_remove_guild_member_role(client, event->guild_id, user_id, role_id, &params, NULL);
}

void done_get_guild_roles(struct discord *client, struct discord_response *resp, const struct discord_roles *roles)
{
    char text[DISCORD_MAX_MESSAGE_LEN];

    char *cur = text;
    char *end = &text[sizeof(text) - 1];
    char *prev;

    for (int i = 0; i < roles->size; ++i)
    {
        prev = cur;
        cur += snprintf(cur, end - cur, "<@&%" PRIu64 ">(%" PRIu64 " -> %s\n", roles->array[i].id, roles->array[i].id, roles->array[i].name);
        if (cur >= end)
        {                 // to make sure no role is skipped
            *prev = '\0'; // end string before last role
            cur = text;   // reset
            --i;          // retry with same role
            continue;
        }
    }

    log_info("Roles:\n%s", text);
}

void fail_get_guild_roles(struct discord *client, struct discord_response *resp)
{
    log_error("Failed to get guild roles: %s", discord_strerror(resp->code, client));
}

void on_role_list(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    struct discord_ret_roles ret =
        {
            .done = &done_get_guild_roles,
            .fail = fail_get_guild_roles,
        };

    discord_get_guild_roles(client, event->guild_id, &ret);
}

void done_get_guild_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
    (void)resp;
    log_info("Member: %s#%s (%" PRIu64 ")", member->user->username, member->user->discriminator, member->user->id);
}

void fail_get_guild_member(struct discord *client, struct discord_response *resp)
{
    log_error("Failed to get guild member: %s", discord_strerror(resp->code, client));
}

void on_member_get(struct discord *client, const struct discord_message *event)
{
    if (event->author->bot)
        return;

    u64snowflake user_id = 0;

    sscanf(event->content, "%" SCNu64, &user_id);

    if (!user_id)
    {
        log_error("Invalid format for `prefix.member_get <user_id>`");
        return;
    }

    struct discord_ret_guild_member ret =
        {
            .done = &done_get_guild_member,
            .fail = &fail_get_guild_member,
        };

    discord_get_guild_member(client, event->guild_id, user_id, &ret);
}

#endif // !BOT_H