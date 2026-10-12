import { LEVEL_LABELS } from '../core/config.js';
import { canonicalLevel } from '../core/legacy-state.js';
export function description(state) {
    const links = [['easy', '🟢', 'EASY'], ['medium', '🟠', 'MEDIUM'], ['hardcore', '🔴', 'HARDCORE'], ['extreme', '🟣', 'EXTREME']]
        .filter(([level]) => state.files.some(file => canonicalLevel(file.role) === level))
        .map(([level, emoji, label]) => {
            const file = state.files.find(file => canonicalLevel(file.role) === level);
            return file ? `## ${emoji} [${label}](${file.url})` : '';
        }).filter(Boolean).join('\n\n');
    const answers = state.previous ? `\n\n<details>\n<summary>Previous round answers · Round ${state.previous.round}</summary>\n\n${Object.entries(state.previous.answers).map(([level, password]) => `- **${LEVEL_LABELS[canonicalLevel(level)]}**: \`${password}\``).join('\n')}\n\n</details>` : '';
    const source = /^[a-f0-9]{40,64}$/.test(state.commit || '') ? ` · [Build source](https://github.com/DosX-dev/obfus.h/tree/${state.commit})` : '';
    return `Round **${state.round}** · **${state.date}** · Windows x64${source}\n\n${links}\n\nRecover the password. Patching does not count.\n\nNew round daily at **13:00 Moscow time (UTC+3)**. Save your downloads.${answers}\n`;
}
