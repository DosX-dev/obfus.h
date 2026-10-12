export function description(state) {
    const links = [['easy', '🟢', 'EASY'], ['medium', '🟠', 'MEDIUM'], ['hard', '🔴', 'HARD']]
        .map(([level, emoji, label]) => {
            const file = state.files.find(file => file.role === level);
            return file ? `## ${emoji} [${label}](${file.url})` : '';
        }).filter(Boolean).join('\n\n');
    const answers = state.previous ? `\n\n<details>\n<summary>Previous round answers · Round ${state.previous.round}</summary>\n\n${Object.entries(state.previous.answers).map(([level, password]) => `- **${level.toUpperCase()}**: \`${password}\``).join('\n')}\n\n</details>` : '';
    return `Round **${state.round}** · **${state.date}** · Windows x64\n\n${links}\n\nRecover the password. Patching does not count.\n\nNew round daily at **13:00 Moscow time (UTC+3)**. Save your downloads.${answers}\n`;
}
