export function description(state) {
    const answers = state.previous
        ? `\n## Previous round ${state.previous.round} (${state.previous.date})\n\n${Object.entries(
            state.previous.answers
        )
            .map(([level, password]) => `- **${level}**: \`${password}\``)
            .join('\n')}\n`
        : '\nThis is the first round. No previous answers yet.\n';
    return `# Daily Crackmes — Round ${state.round}\n\nGenerated: **${state.date}** · Windows x64\n\nNew rounds daily at **13:00 Moscow time (UTC+3)**. Publication may be delayed.\n\nRecover the original password. Patching the success message does not count.\n\n${state.files
        .filter((f) => f.role !== 'answers')
        .map((f) => `- [${f.name}](${f.url})`)
        .join(
            '\n'
        )}\n${answers}\nOnly the current round is retained. Save downloaded challenges if you want to finish later.\n`;
}
